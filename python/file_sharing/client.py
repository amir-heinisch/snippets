#!/usr/bin/python3.9

"""

file:		client.py
author:		Amir Heinisch <mail@amir-heinisch.de>
version:	16/08/2021

"""

import readline
import sys
from commands import SERVER_COMMAND
from socket import socket, AF_INET, SOCK_STREAM
from utils import HOST, PORT

sock = None

def connect():
	global sock
	sock = socket(AF_INET, SOCK_STREAM)
	sock.setblocking(True)
	sock.connect((HOST, PORT))

	print(f'Client connected to server ({HOST}:{str(PORT)})')

def interactive():
	global sock
	# Enable tab completion.
	readline.parse_and_bind('tab: complete')
	readline.set_completer(SERVER_COMMAND(1).complete)

	while True:
		print('Please select a command:')
		SERVER_COMMAND.print_commands()

		# Get user input.
		inp = input('> ')

		if inp.isdecimal(): # Interpret input as index number.
			selected_command = SERVER_COMMAND(int(inp))
		else:
			selected_command = SERVER_COMMAND[inp]

		print(f'Requested command: {selected_command.name}')
		selected_command.request(sock)

def scenario():
	global sock

	command = SERVER_COMMAND.LIST_FILES
	command.request(sock)

	command = SERVER_COMMAND.UPLOAD
	command.request(sock, 'The_file.jpg')

	command = SERVER_COMMAND.DOWNLOAD
	command.request(sock, 'The_file.jpg')

if __name__ == '__main__':
	try:
		# Connect to server.
		connect()

		# Interactive client.
		if len(sys.argv) > 1 and sys.argv[1] == '-i':
			interactive()
		else:
			scenario()

	except KeyboardInterrupt: # Quit with ctrl+c.
		# NOTE: No special disconnect msg required.
		pass
	except Exception as e:
		print(e)
	finally:
		if sock:
			sock.close()

