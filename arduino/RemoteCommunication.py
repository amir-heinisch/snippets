#!/usr/bin/python3.9

"""

filename:	RemoteCommunication.py
author:		Amir Heinisch <mail@amir-heinisch.de>
version:	16/08/2021

This python script allows us to establish a remote bidirection communication between
two microcontroller running our morse pipeline.
Therefore we simply read words from serial and upload them into the other boards inbox via a
simple webserver (plain http and files).

This script works with a simple nginx server config:

server {
	listen			ip;
	server_name		server.de;

	access_log		/var/log/nginx/is.log;
	error_log		/var/log/nginx/error_is.log debug;

	auth_basic				"Basic auth";
	auth_basic_user_file	/etc/nginx/htpasswd;

	location ^~ \.php$ {
		deny all;
	}

	location ^~ / {
		root	/usr/share/nginx/html/is;
		index	index.html index.html;

		dav_methods		PUT DELETE;
	}
}

"""

from http.client import HTTPConnection
from base64 import b64encode
from time import sleep
from threading import Thread
from os import openpty, ttyname , read, write, open, O_RDWR
from select import poll, POLLIN, POLLOUT

class tcol:
	GREEN = '\033[92m'
	RED = '\033[91m'
	END = '\033[0m'

class RemoteHelper:

	tty = ''
	board_id = ''
	other_ids = []
	connection = None
	log_color = None

	def __init__(self, tty, board_id, other_ids, log_color):
		self.tty = tty
		self.board_id = board_id
		self.other_ids = other_ids
		self.log_color = log_color

		# Make sure there are no old inboxes.
		self.delete_inbox()
		# Create new inbox
		self.create_inbox()

	def log(self, msg):
		print(self.log_color + msg + tcol.END)

	def request(self, method='GET', msg='', bid='default'):
		self.connection = HTTPConnection('server.com', 80, timeout=10)
		if not self.connection:
			self.log('ERROR: no connection available.')
			return None

		# Execute authenticated request.
		self.connection.request(method, '/{}.inbox'.format(bid), msg, headers={
			'Authorization' : 'Basic {}'.format(b64encode(b'user:pass').decode('ascii'))
		})
		response = self.connection.getresponse()

		# Handle unexpected error codes.
		if (method == 'GET' and response.status in [200, 400]) or \
				(method == 'PUT' and response.status in [201, 204]) or \
				(method == 'DELETE' and response.status in [200, 204, 404]):
					return response
		else:
			self.log('ERROR: {} (reason: {})'.format(response.status, response.reason))
			return None

	def create_inbox(self):
		if self.request('PUT', bid=self.board_id):
			self.log('Successfully created inbox on server.')

	def delete_inbox(self):
		if self.request('DELETE', bid=self.board_id):
			self.log('Successfully deleted inbox from server.')

	def clear_inbox(self):
		if self.request('PUT', bid=self.board_id):
			self.log('Successfully created inbox on server.')

	def send_message(self, msg):
		fails = []
		for bid in self.other_ids:
			response = self.request(bid=bid)
			# Check if mailbox is available and empty.
			# Only log if inbox is empty (NOTE! Races with more then two parties)
			if response.status == 200 and response.read() == b'':
				if self.request('PUT', msg=msg, bid=bid):
					self.log(f'Successfully send message "{msg}" to {bid}.')
				else:
					fails.append(bid)
		return fails

	def receive_message(self):
		response = self.request(bid=self.board_id)
		if response: # TODO: what if file is not available?
			message = response.read().decode('ascii')
			if message:
				self.log("board " + self.board_id + " inbox: ")
				self.clear_inbox()
				return message
		return None

	def read_serial_message(self):
		# TODO: dumb but works..
		poller = poll()
		poller.register(self.tty, POLLIN)
		if poller.poll(0):
			return read(self.tty, 2048)

	def write_serial_message(self, msg):
		# TODO: dumb but works..
		poller = poll()
		poller.register(self.tty, POLLOUT)
		if poller.poll(0):
			write(self.tty, bytes(msg, 'ascii'))

	def run(self):
		round = 0

		while True:
			self.log(f'[Round: {round}]')

			# Receive msg from inbox.
			self.log('Check webserver inbox:')
			msg = self.receive_message()

			# Handle received messages.
			if msg:
				self.log(f'  Message: {msg}')
				self.write_serial_message(msg)
			else:
				self.log('  Empty inbox.')

			# Read msg from serial.
			self.log('Check serial input:')
			serial_msg = self.read_serial_message()

			# Handle received messages.
			if serial_msg:
				self.log(f'  Serial message: {serial_msg}')
				fails = self.send_message(serial_msg) # TODO: handle send fails.
			else:
				self.log('  No input.')

			round += 1
			sleep(1)

def main():

	if False: # True = remote setup.
		board = None
		try:
			tty = open('/dev/ttyUSB0', O_RDWR)
			board = RemoteHelper(tty, 'b1', ['b2'], tcol.GREEN)
			board.run()
			while True:
				pass
		except KeyboardInterrupt:
			pass
		except Exception as e:
			print(e)
			pass
		finally:
			if board:
				board.delete_inbox()

	elif False: # True = local setup.
		m1, s1 = openpty()
		m2, s2 = openpty()
		print('board b1: {}'.format(ttyname(s1)))
		print('board b2: {}'.format(ttyname(s2)))

		b1 = RemoteHelper(m1, 'b1', ['b2'], tcol.GREEN)
		b2 = RemoteHelper(m2, 'b2', ['b1'], tcol.RED)

		# Create and start threads
		t1 = Thread(target=b1.run)
		t1.daemon = True
		t1.start()
		t2 = Thread(target=b2.run)
		t2.daemon = True
		t2.start()

		try:
			while True:
				pass
		except KeyboardInterrupt:
			pass
		finally:
			b1.delete_inbox()
			b2.delete_inbox()
	else: # Hybrid testing setup.
		"""
			You can simply use this setup to test the communication locally.
			Connect your arduino and insert the correct tty path.
			Setup the nginx server as stated in the RemoteCommunication.py and
			Run the RemoteCommunication.py script.
		"""
		m1, s1 = openpty()
		print('board b1: {}'.format(ttyname(s1)))
		b1 = RemoteHelper(m1, 'b2', ['b1'], tcol.GREEN)
		#tty = open('/dev/ttyUSB0', O_RDWR)
		#b2 = RemoteHelper(tty, 'b2', ['b1'], tcol.RED)

		# Create and start threads
		t1 = Thread(target=b1.run)
		t1.daemon = True
		t1.start()
		#t2 = Thread(target=b2.run)
		#t2.daemon = True
		#t2.start()

		try:
			while True:
				pass
		except KeyboardInterrupt:
			pass
		finally:
			b1.delete_inbox()
			#b2.delete_inbox()

if __name__ == '__main__':
	main()
