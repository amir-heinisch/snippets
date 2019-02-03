#!/usr/bin/env python3.7

""" This is a little shell in python to learn the concepts. """

import os
from subprocess import call

"""
    TODO:
        - Allow real configuration
        - wrapper, secure, honeypod modes
        - Save log to file
        - Enable tab completion
        - vim like navigation
"""

def get_prompt():
    """
        This method should create the wanted prompt.
    """

    return "[user@computer " + os.getcwd() + "]$ "

def handle_input(inp):
    """
        This method should handle all input.
    """

    if inp[0] == "exit":
        print("exit")
        exit()
    elif inp[0] == "ls": 
        # Just as example for secure mode.
        call(['ls'])
    elif inp[0] == "nc":
        # Just as example for honepod mode.
        print("Ha nooo")
    else:
        call(inp)

    return

def log(inp):
    """
        Log given input to history file.
    """
    pass

def main():
    # Run setup script
    # Given via env var or in current dir.
    setup_script = os.getenv('SHELL_SETUP_SCRIPT')
    if setup_script:
        print("Run setup script from env var.")
        call(['python3.7', str(setup_script)]);
    else:
        # call(['python3.7', '.shell_setup.py', '2>&1 > /dev/null'])
        pass
    
    while True:
        # Read new command.
        inp = input(get_prompt())

        # Check if there is some input.
        if not inp:
            continue

        # Add input to history file.
        log(inp)

        # Handle input now.
        handle_input(inp.split(' '))

if __name__ == '__main__':
    # Run shell.
    main()
