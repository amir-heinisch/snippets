"""
    This attacker type is just running the control loops with random values.
"""

from attacker.attacker import Attacker
from utils import toLower

import importlib
import logging
import random

__author__  = 'Amir Heinisch'
__email__   = 'mail@amir-heinisch.de'
__date__    = '2019/12/11'

class RandomAttacker(Attacker):

    def attack(self):
        """ This attack runs the control loops with random values. """

        logging.debug("RandomAttacker attack started...")

        # Print initial output line.
        self.output(','.join(self.process.stateNames) + "\n")

        while True:
            processState = self.process.state

            if self.defender:
                if self.process.round < self.defender.learningRounds:
                    self.defender.learn(processState)
                else:
                    self.defender.detect(processState)

            # Create random state.
            randomState = []
            for i in range(len(processState)):
                # Calc a random value within a range of 4% of the current value.
                # TODO: maybe there are precision problems in python?
                cv = processState[i]
                th = cv * 0.04
                randomState.append(random.uniform(cv-th, cv+th))

            for loop in self.process.controlLoops:
                brokenState = loop(randomState.copy())
                for i in range(len(brokenState)):
                    if brokenState[i] != randomState[i]:
                        processState[i] = brokenState[i]

            # Set the current state.
            self.process.state = processState
            # Run the next simulation round.
            self.process.simulate()

            # Output sensor updates.
            self.output(str(self.process.round) + ":" + str(self.process.state).lstrip('[').rstrip(']') + "\n")
