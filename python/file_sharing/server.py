#!/usr/bin/python3.9

"""

file:		server.py
author:		Amir Heinisch <mail@amir-heinisch.de>
version:	16/08/2021

"""

from utils import HOST, PORT, recv_until
from commands import SERVER_COMMAND
from socket import socket, AF_INET, SOCK_STREAM

def run(host, port):
	sock = None
	try:
		sock = socket(AF_INET, SOCK_STREAM)
		sock.setblocking(True)
		sock.bind((host, port))
		sock.listen(1)

		print(f'Server is listening ({host}:{str(port)})')

		while True: # Handle on client after the other.
			conn, addr = sock.accept()
			print(f'Client connected ({addr[0]}:{addr[1]})')

			while True: # Handle current client until connection is over.
				command = recv_until(conn,
					SERVER_COMMAND.max_len_command(), SERVER_COMMAND.values())

				if command == None:
					print(f'Connection broke or closed by client.')
					break
				elif command == '':
					print(f'Requested invalid command: {command}')
					continue

				print(f'Requested command: {command}')
				requested_command = SERVER_COMMAND[command]
				requested_command.respond(conn)

	except KeyboardInterrupt: # Quit with ctrl+c.
		pass
	except Exception as e:
		print('server.py: Exception fired!')
		print(type(e))
		print(e.args)
	finally:
		if sock:
			sock.close()


if __name__ == '__main__':
	# Start server.
	run(HOST, PORT)
