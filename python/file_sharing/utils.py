"""

file:		utils.py
author: 	Amir Heinisch <mail@amir-heinisch.de>
version:	16/08/2021

"""

import hashlib
import re
from os import getcwd

HOST = '127.0.0.1'
PORT = 7778
TIMEOUT = 0.5
DIR = getcwd()

## Helper ##

def generate_md5_hash(data):
	return str(hashlib.md5(data).hexdigest())

def fid(file):
	with open(file, 'rb') as f:
		return generate_md5_hash(f.read())

def recv_until(sock, byte_count, matches=[], decoding='ascii'):
	data = b''
	received_bytes = b''
	while True:
		missing_bytes = (byte_count + 1) - len(data) # Enable nc (adds \n).
		received_bytes = sock.recv(missing_bytes)

		if len(received_bytes) < 1: # Connection broke
			return None

		data += received_bytes

		recvd = data
		if decoding:
			recvd = data.decode(decoding)
			# Remove last newline (nc adds one).
			recvd = recvd.removesuffix('\n')

		if matches: # Returns on first match.
			if isinstance(matches, str): # Use regex.
				pattern = re.compile(matches)
				if pattern.match(recvd):
					return recvd
			else: # Match string in list.
				if recvd in matches:
					return recvd
				elif len(recvd) >= byte_count:
					return ''
		elif len(recvd) >= byte_count:
			return recvd

