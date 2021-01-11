"""
    This defender simply watches the process values
    for a given amount of time and saves the min and max
    values for each variable.
    If the process values leave this in the detection
    phase an exception will be raised.
"""

from defender.defender import Defender

__author__  = 'Amir Heinisch'
__email__   = 'mail@amir-heinisch.de'
__date__    = '2020/03/22'

class RangeDefender(Defender):

    # List of min values
    minValues = None

    # List of max values
    maxValues = None

    def learn(self, processValues):
        """ Find out the range for each variable. """

        # Init min max arrays first if needed.
        if self.minValues is None or self.maxValues is None:
            self.minValues = [0] * len(processValues)
            self.maxValues = self.minValues.copy()

        for i in range(len(processValues)):
            v = processValues[i]
            if v < self.minValues[i]:
                self.minValues[i] = v
            if v > self.maxValues[i]:
                self.maxValues[i] = v

    def detect(self, processValues):
        """ Check if values stay in there.
        """

        # Init min max arrays first if needed.
        if self.minValues is None or self.maxValues is None:
            self.minValues = [0] * len(processValues)
            self.maxValues = self.minValues.copy()

        for i in range(len(processValues)):
            v = processValues[i]
            if v < self.minValues[i]:
                self.attackDetected("Value out of learned range.")
            if v > self.maxValues[i]:
                self.attackDetected("Value out of learned range.")
