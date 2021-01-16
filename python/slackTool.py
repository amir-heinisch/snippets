#!/usr/bin/env python3

"""

filename: slackTool.py
description: This tool allows us to read and write file slack space.
author: Amir Heinisch <mail@amir-heinisch.de>
version: 16/01/2021

"""

import os
import sys
import logging
import struct
from fcntl import *

# NOTE: Read until newline.
def read_slack(target_dev_file):
    print(target_dev_file.readline().decode(errors='ignore'), end='')

def wipe_slack(target_dev_file, slack_size):
    write_slack(target_dev_file, slack_size, b'\00'*slack_size)

def write_slack(target_dev_file, slack_size, inp=''):
    if not inp:
        inp = input('Enter data to hide in slack (max {} characters): '.format(slack_size)).encode()
    if (len(inp) > slack_size):
        logging.error('Max {} characters are allowed!'.format(slack_size))
    target_dev_file.write(inp + b'\n')
    logging.info("Data to hidden: {}".format(inp))

def main(argc, argv):

    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

    if argc != 3:
        logging.error('Usage: python3 {} [r(ead) | w(rite)] <filename>'.format(argv[0]))
        return

    target_filename = argv[2]
    target_start_block = 0
    # Get target file stats.
    stat = os.stat(target_filename)
    # Get target filesize.
    target_filesize = stat.st_size
    # Get target file dev minar and major numbers.
    major, minor = os.major(stat.st_dev), os.minor(stat.st_dev)
    # Get default fs block size.
    fs_block_size = stat.st_blksize

    logging.debug('Use file: {}'.format(target_filename))

    # Get device filename for file.
    target_dev_filename = os.path.realpath('/dev/block/{}:{}'.format(major,minor))
    if not os.path.exists(target_dev_filename):
        logging.error('Can not find target device file.')
        return
    logging.debug('Device filename: ', target_dev_filename)

    with open(target_filename, 'r') as target_file:
        res = ioctl(target_file, 1, struct.pack('I', 0)) # FIBMAP
        target_start_block = struct.unpack('I', res)[0]
    target_file.close()

    with open(target_dev_filename, 'wb+') as target_dev_file:
        # NOTE! For ppc its 536870914 instead of 2 (FIGETBSZ)
        res = ioctl(target_dev_file, 2, struct.pack('I', 0))
        fs_block_size = struct.unpack('I', res)[0]
        logging.debug('Filesystem block size: {}'.format(fs_block_size))

        # Calculate slack size.
        if target_filesize % fs_block_size != 0:
            slack_size = fs_block_size - (target_filesize % fs_block_size)
        # Calculate offset.
        offset = target_start_block * fs_block_size + target_filesize % fs_block_size

        # Move cursor.
        target_dev_file.seek(offset)

        logging.info('getting from block {}'.format(str(target_start_block)))
        logging.info('file size was: {}'.format(str(target_filesize)))
        logging.info('slack size: {}'.format(str(slack_size)))
        logging.info('block size: {}'.format(str(fs_block_size)))

        if argv[1] == 'read' or argv[1] == 'r':
            read_slack(target_dev_file)
        elif argv[1] == 'write' or argv[1] == 'w':
            write_slack(target_dev_file, slack_size)
        elif argv[1] == 'wipe' or argv[1] == 'wi':
            wipe_slack(target_dev_file, slack_size)
        else:
            logging.error('Selected mode is unknown!')

    target_dev_file.close()

if __name__ == '__main__':
    main(len(sys.argv), sys.argv)
