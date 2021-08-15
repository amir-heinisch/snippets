"""

file:		commands.py
author:		Amir Heinisch <mail@amir-heinisch.de>
version:	16/08/2021

"""

import enum
import re
import select
from os import getcwd, listdir
from os.path import join, isfile, getsize
from utils import TIMEOUT, DIR, fid, recv_until

## Requests ##

def list_files(sock, printout=True):
	# Send command.
	sent = sock.sendall(SERVER_COMMAND.LIST_FILES.command.name.encode('ascii'))
	if sent == 0:
		raise RuntimeError('Socket connection broken')

	# Receive response.
	data = []
	while True:
		received_bytes = b''

		# NOTE:	Since there can be a huge and variable number of files
		# 		in a directory, we receive until a given timeout.
		ready = select.select([sock], [], [], TIMEOUT)
		if ready[0]:
			received_bytes = sock.recv(4096)
			if received_bytes == b'': # Connection broke.
				break
		elif data:
			break

		data.append(received_bytes)

	response = (b''.join(data)).decode('ascii')

	# Check format of received data.
	pattern = re.compile(
		'^([a-fA-f0-9]{32}[;][a-zA-Z0-9._]{3,255}[;][0-9]+[\r\n]*)+$')
	if not 'No files available at the moment.' in response and \
		not pattern.match(response):
		raise RuntimeError('Protocol violation!')

	if printout:
		print(response)
	else:
		return response

def download(sock, filename=None):
	# Execute LIST_FILES command before to get filenames, sizes and ids.
	command = SERVER_COMMAND.LIST_FILES
	files = command.request(sock, False)

	msg = 'No files available at the moment.\n'
	if msg == files:
		print(msg)
		return

	# NOTE: Parsing should work..string matched regex before.
	files = list(map(lambda x: x.split(';'), files.split('\n')[:-1]))

	# Execute download command.
	sent = sock.sendall(SERVER_COMMAND.DOWNLOAD.command.name.encode('ascii'))
	if sent == 0:
		raise RuntimeError('Socket connection broken')

	# Handle ask for file id.
	msg = 'file_id'
	if not recv_until(sock, len(msg), [msg]):
		raise RuntimeError('Protocol violation!')

	# Get file id and size.
	file_id = None
	file_size = None
	if not filename:
		filename = input('Filename: ')
	files = list(filter(lambda x: filename == x[1], files))
	if len(files) > 0:
		file_id = files[0][0]
		file_size = int(files[0][2])
	else:
		raise RuntimeError('File does not exist!')

	# Send file id.
	sent = sock.sendall(file_id.encode('ascii'))
	if sent == 0:
		raise RuntimeError('Socket connection broken')

	# Handle file byte stream.
	response = recv_until(sock, file_size, decoding=None)
	if not response:
		raise RuntimeError('Protocol violation!')
	print('-> Receiveing ', end='')
	print(response[:20], end='')
	print('...')

	if isfile(filename): # NOTE: not specified.
		filename = filename + '.download'

	with open(filename, 'wb') as f:
		f.write(response)

	if file_id == fid(filename):
		print('SUCCESS! File is not currupted..')
	else:
		print('ERROR! File is corrupted..')

def upload(sock, filename=None):
	# Send command.
	sent = sock.sendall(SERVER_COMMAND.UPLOAD.command.name.encode('ascii'))
	if sent == 0:
		raise RuntimeError('Socket connection broken')

	# Handle ask for filename and filesize.
	msg = 'filename_filesize'
	if not recv_until(sock, len(msg), [msg]):
		raise RuntimeError('Protocol violation!')

	# Send file name and size.
	if not filename:
		filename = input('Filename: ')
	filesize = getsize(filename)
	sent = sock.sendall((filename + ';' + str(filesize)).encode('ascii'))
	if sent == 0:
		raise RuntimeError('Socket connection broken')

	# Handle upload begin.
	msg = 'ready to receive a file'
	if not recv_until(sock, len(msg), [msg]):
		raise RuntimeError('Protocol violation!')

	# Upload
	with open(filename, 'rb') as f:
		print(f'-> Uploading file ({filename})')
		sock.sendall(f.read())

	# Verify upload.
	file_id = recv_until(sock, 32) # md5 -> 128 bit = 32 hex.
	if file_id == fid(filename):
		print('SUCCESS! File is not currupted..')
	else:
		print('ERROR! File is corrupted..')


## Responses ##

def response_list_files(sock):
	response = ('\n'.join(
		[
			fid(f) + ';' + f + ';' + str(getsize(f)) for f in listdir(DIR)
			if isfile(join(DIR, f))
		]
	) or 'No files available at the moment.' ) + '\n'

	sock.sendall(response.encode('ascii'))

def response_download(sock):
	# Ask for file id.
	sock.sendall('file_id'.encode('ascii'))

	file_id = recv_until(sock, 32, '^[a-zA-Z0-9]{32}$')
	if not file_id:
		raise RuntimeError('Protocol violation!')

	for p in listdir(DIR):
		path = join(DIR, p)
		if isfile(path) and fid(path) == file_id:
			with open(path, 'rb') as f:
				print(f'-> Sending file ({path})')
				sock.sendall(f.read())
				return

	raise RuntimeError('Invalid file id!')


def response_upload(sock):
	# Send question for filename and filesize.
	sent = sock.sendall('filename_filesize'.encode('ascii'))
	if sent == 0:
		raise RuntimeError('Socket connection broken')

	# Handle ask for filename and filesize.
	# TODO: handle path traversel attacks.
	response = recv_until(sock, 300, '^[a-zA-Z0-9._]{3,255}[;][0-9]+$')
	if not response:
		raise RuntimeError('Protocol violation!')
	filename, filesize = response.split(';')
	filesize = int(filesize)

	# Send ready to receive.
	sent = sock.sendall('ready to receive a file'.encode('ascii'))
	if sent == 0:
		raise RuntimeError('Socket connection broken')

	# Handle file byte stream.
	response = recv_until(sock, filesize, decoding=None)
	if not response:
		raise RuntimeError('Protocol violation!')
	print('-> Receiveing ', end='')
	print(response[:20], end='')
	print('...')

	if isfile(filename): # NOTE: not specified.
		filename = filename + '.upload'

	# Write file.
	with open(filename, 'wb') as f:
		f.write(response)

	# Send file id.
	sent = sock.sendall(fid(filename).encode('ascii'))
	if sent == 0:
		raise RuntimeError('Socket connection broken')


## Command abstraction ##

class Command:
	def __init__(self, name, request, response):
		self.name = name
		self.request = request
		self.respond = response


class SERVER_COMMAND(enum.Enum):
	LIST_FILES = Command('LIST_FILES', list_files, response_list_files)
	DOWNLOAD = Command('DOWNLOAD', download, response_download)
	UPLOAD = Command('UPLOAD', upload, response_upload)

	def __new__(cls, command):
		next_idx = len(cls.__members__) + 1
		obj = object.__new__(cls)
		obj._value_ = next_idx
		obj.command = command
		return obj

	def request(self, *args):
		return self.command.request(*args)

	def respond(self, *args):
		return self.command.respond(*args)

	def complete(self, text, state):
		""" Readline tab completion """
		results = [ c.name for c in SERVER_COMMAND if c.name.startswith(text) ]
		results += [None]
		return results[state]

	@staticmethod
	def print_commands():
		line = [ f'[{x.value}] {x.name}' for x in SERVER_COMMAND ]
		print('\n'.join(line))

	@staticmethod
	def values():
		return [ v.name for v in SERVER_COMMAND]

	@staticmethod
	def max_len_command():
		max_command_len = 0
		for val in SERVER_COMMAND:
			if len(val.name) > max_command_len:
				max_command_len = len(val.name)
		return max_command_len

