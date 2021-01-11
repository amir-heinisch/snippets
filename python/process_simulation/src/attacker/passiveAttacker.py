"""
    This attacker just runs the process and writes all values to file.
    If you want to write your own attacker just follow this implementation.
    Maybe the comments can help you.
"""

from attacker.attacker import Attacker

import importlib
import logging
from utils import toLower

__author__  = 'Amir Heinisch'
__email__   = 'mail@amir-heinisch.de'
__date__    = '2019/11/15'

class PassiveAttacker(Attacker):

    def attack(self):
        """
            This method implements the 'attack', which only monitors the
            plain process.
        """

        logging.debug("PassiveAttacker attack started...")

        # Print initial output line.
        # NOTE! The webui takes the first line for naming the differnt values
        # in the frontend. This makes reading the graph more easy.
        self.output(','.join(self.process.stateNames) + "\n")

        while True:
            # Take the current process state.
            # NOTE! The process needs to initialize this field in its
            #       constructor.
            processState = self.process.state

            # Run defender if wanted.
            # NOTE! If no attacker is configured in the config file then we
            #       didn't init one, so we don't need to run him now.
            if self.defender:
                # Each Defender has a phase to learn.
                # NOTE! Your real attack should not start before this phase if
                #       over. Just run the process like here until this point.
                if self.process.round < self.defender.learningRounds:
                    self.defender.learn(processState)
                else:
                    self.defender.detect(processState)

            # Just run proccess as intended.
            # NOTE! We run the plc logic in the registered order.
            #       Each loop works with the outcome of the prev one.
            # NOTE! This is just for processes with a distributed control
            #       algorithm. Just dont add functions to the array if not
            #       needed.
            for loop in self.process.controlLoops:
                processState = loop(processState)
            # Set the current state.
            self.process.state = processState
            # Run the next simulation round.
            self.process.simulate()

            # Output sensor updates.
            self.output(str(self.process.round) + ":" + str(self.process.state).lstrip('[').rstrip(']') + "\n")
