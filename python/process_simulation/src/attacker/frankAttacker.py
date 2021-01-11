"""
    This attacker type is just trying to rebuild some
    of franks attacks.
"""

from attacker.attacker import Attacker
from utils import toLower

import importlib
import logging

__author__  = 'Amir Heinisch'
__email__   = 'mail@amir-heinisch.de'
__date__    = '22/01/2020'

class FrankAttacker(Attacker):

    attackCode = 1

    def __init__(self, config):
        super().__init__(config)

        if 'attackCode' in config:
            # TODO: handle error.
            self.attackCode = int(config['attackCode'])

    def attack(self):
        """ This 'attack' rebuilds some of franks attacks. """

        logging.debug("FrankAttacker attack started...")

        # Print initial output line.
        self.output(','.join(self.process.stateNames) + "\n")

        while True:
            if self.attackCode == 1:
                self.zeroReplaceReceive()
            elif self.attackCode == 2:
                self.ignoreConnections()
            else:
                logging.info("Attack code unknown.")

            # Output sensor updates.
            self.output(str(self.process.round) + ":" + str(self.process.state).lstrip('[').rstrip(']') + "\n")

    def zeroReplaceReceive(self):
        """ This attack just set all receive values to zero and should succed after 206 iterations with high pressure shutdown. """

        # The following values can cause a shutdown:
        # 1: High Reactor Pressure          - *tcModelData.pv_.xmeas[6] > (float)3e3
        # 2: High Reactor Liquid Level      - *tcModelData.teproc_.vlr / (float)35.3145 > (float)24.
        # 3: Low Reactor Liquid Level       - *tcModelData.teproc_.vlr / (float)35.3145 < (float)2.
        # 4: High Reactor Temperature       - *tcModelData.pv_.xmeas[8] > (float)175.
        # 5: High Separator Liquid Level    - *tcModelData.teproc_.vls / (float)35.3145 > (float)12.
        # 6: Low Separator Liquid Level     - *tcModelData.teproc_.vls / (float)35.3145 < (float)1.
        # 7: High Stripper Liquid Level     - *tcModelData.teproc_.vlc / (float)35.3145 > (float)8.
        # 8: Low Stripper Liquid Level      - *tcModelData.teproc_.vlc / (float)35.3145 < (float)1.

        processState = self.process.state
        manipulatedState = processState.copy()

        if self.defender:
            if self.process.round < self.defender.learningRounds:
                self.defender.learn(processState)
            else:
                self.defender.detect(processState)

        val = 0.

        # plc1
        manipulatedState[16] = val
        manipulatedState[39] = val
        # plc2
        manipulatedState[7] = val
        manipulatedState[10] = val
        # plc3
        manipulatedState[16] = val
        # plc4
        manipulatedState[6] = val
        manipulatedState[16] = val
        #plc5: no receives.

        # Run next iteration.
        for loop in self.process.controlLoops:
            manipulatedState = loop(manipulatedState)
        # Set the current state.
        # TODO: do this more generic..look at random attacker
        self.process.state = processState[:41] + manipulatedState[41:]
        # Run the next simulation round.
        self.process.simulate()

    def ignoreConnections(self):
        """ This attack ignores some connections between plcs. """

        processState = self.process.state
        manipulatedState = processState.copy()

        if self.defender:
            if self.process.round < self.defender.learningRounds:
                self.defender.learn(processState)
            else:
                self.defender.detect(processState)

        val = 0.

        # plc1
        manipulatedState[16] = val
        manipulatedState[39] = val
        # plc2
        manipulatedState[7] = val
        # manipulatedState[10] = val
        # plc3
        manipulatedState[16] = val
        # plc4
        manipulatedState[6] = val
        manipulatedState[16] = val
        #plc5: no receives.

        # Run next iteration.
        for loop in self.process.controlLoops:
            manipulatedState = loop(manipulatedState)
        # Set the current state.
        # TODO: do this more generic..look at random attacker
        self.process.state = processState[:41] + manipulatedState[41:]
        # Run the next simulation round.
        self.process.simulate()
