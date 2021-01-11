#!/usr/bin/env python

""" Entrypoint of this project (cli interface). """

from utils import toLower, sanitizeClass

import sys
import pathlib
import argparse
import importlib
import logging
import random
import configparser
import subprocess
import os.path

__author__  = 'Amir Heinisch'
__email__   = 'mail@amir-heinisch.de'
__date__    = '2019/11/15'

def setupLogging(logpath='./logfile.log', quiet=False, debug=False):
    """ This methods should setup the needed logging. """

    level = logging.DEBUG if debug else logging.INFO
    format = "%(asctime)-8s [%(filename)s:%(lineno)d] [%(levelname)s] : %(message)s"
    dateFormat = "%H:%M:%S"
    
    # Set handlers for logger
    handlers = []
    if not quiet:
        handlers.append(logging.StreamHandler()) # sys.stdout
    if logpath:
        # TODO: check filename...
        handlers.append(logging.FileHandler(logpath))
    else:
        # Indicate empty path.
        logpath = "''"

    logging.basicConfig(format=format, datefmt=dateFormat, level=level, handlers=handlers)
    logging.debug("---------------------------------------------------------------------------------------------------------------------------------------------")
    logging.debug("Logging enabled and configured: level: " + str(level) + ", format: " + format + ", datefmt: " + dateFormat + ", filename: " + str(logpath))

if __name__ == '__main__':

    # Check interpreter version.
    assert sys.version_info >= (3, 8), 'ERROR! We only support python3.8 and newer!'

    # Parse input.
    parser = argparse.ArgumentParser()
    parser.add_argument('config', help='A config file is needed to run.')
    parser.add_argument('-V', '--version', action='version', version='%(prog)s 1.0')
    args = parser.parse_args()

    # Read mandatory values from config file.
    try:
        config = configparser.ConfigParser()
        config.read(args.config)

        # Get needed values from config.
        general = config['General']
        debug = general.getboolean('debug')
        quiet = general.getboolean('quiet')
        logpath = general['logpath']

        attackerConfig = config['Attacker']
        processConfig = config['Process']

        attackerName = attackerConfig['class']

    except:
        print("ERROR! Config file is not valid!")
        sys.exit(1)

    # Create output directory.
    if 'outputPath' in attackerConfig:
        pathlib.Path(os.path.dirname(attackerConfig['outputPath'])).mkdir(exist_ok=True)
    else:
        pathlib.Path('./out').mkdir(exist_ok=True)

    # Setup logging.
    setupLogging(logpath, quiet, debug)

    if 'webui' in general and general.getboolean('webui'):
        # Start webui in the background quietly.
        if 'webuiPort' in general:
            subprocess.Popen(["python", "src/ui/app.py", "127.0.0.1", general['webuiPort'], "out/webui.pid", "out/teRunner.txt" ], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        else:
            subprocess.Popen(["python", "src/ui/app.py", "127.0.0.1", str(5000 + random.randint(0,999)), "out/webui.pid", "out/teRunner.txt" ], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

    attacker = None

    try:
        # Import attacker.
        attackerClass = sanitizeClass(attackerName)
        logging.debug('Try to use attacker ' + attackerClass + ' from module attacker.' + toLower(attackerClass))
        attackerModule = importlib.import_module('attacker.' + toLower(attackerClass), package=__name__)

        # Create attacker.
        attacker_ = getattr(attackerModule, attackerClass)
        attacker = attacker_(attackerConfig)
        logging.debug('Successfully created attacker ' + attackerClass + ' from module attacker.' + toLower(attackerClass) + ' [' + str(attacker) + ']')

        # Let attacker setup process.
        attacker.initializeProcess(processConfig)

        # Setup defender if needed.
        if 'Defender' in config:
            attacker.initializeDefender(config['Defender'])

        # Run attack.
        attacker.startAttack()

    except KeyboardInterrupt as e:
        logging.info("Keyboard interrupt received, stopping..")
    except ImportError as e:
        logging.error("Import error.. (" + str(e) + ")")
        sys.exit(1)
    finally:
        logging.shutdown()

