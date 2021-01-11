"""
    This attacker type is running the process without changing
    process values. This should simulate an attack which breaks
    plcs involved in the process. This attack will only show effects
    on non determenistic simulations.
"""

from attacker.attacker import Attacker
from utils import toLower

import importlib
import logging

__author__  = 'Amir Heinisch'
__email__   = 'mail@amir-heinisch.de'
__date__    = '2019/11/15'

class DosAttacker(Attacker):

    interceptedVars = None
    ignoredVars = None

    def __init__(self, config):
        """
            Init attacker.

            Args:
                config (dict): Configurations for attacker.
        """
        super().__init__(config)

        try:
            if 'interceptedVars' in config:
                self.interceptedVars = [ int(v) for v in config['interceptedVars'].split(',')]
            if 'ignoredVars' in config:
                self.ignoredVars = [ int(v) for v in config['ignoredVars'].split(',')]
        except:
            pass


    def attack(self):
        """
            This 'attack' runs the process with same values,
            which should simulate PLCS don't get new
            values. This makes them print the same values again.
        """

        logging.debug("DosAttacker attack started...")

        # Print initial output line.
        self.output(','.join(self.process.stateNames) + "\n")

        while True:
            processState = self.process.state

            if self.defender:
                if self.process.round < self.defender.learningRounds:
                    self.defender.learn(processState)
                else:
                    self.defender.detect(processState)

            # Do not block all values.
            if self.ignoredVars is not None:
                state = processState.copy()
                for loop in self.process.controlLoops:
                    state = loop(state)
                for i in self.ignoredVars:
                    processState[i] = state[i]

            # Only block some values.
            if self.interceptedVars is not None:
                state = processState.copy()
                for loop in self.process.controlLoops:
                    state = loop(state)
                for i in self.interceptedVars:
                    state[i] = processState[i]
                processState = state

            self.process.state = processState
            self.process.simulate()

            # Output sensor updates.
            self.output(str(self.process.round) + ":" + str(self.process.state).lstrip('[').rstrip(']') + "\n")
