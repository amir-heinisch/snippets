"""
    This defender simply watches the process values
    for a given amount of time and calculates the
    median for each variable.
    If the process values leave a configured treshhold
    in the detection phase an exception will be raised.
"""

from defender.defender import Defender

__author__  = 'Amir Heinisch'
__email__   = 'mail@amir-heinisch.de'
__date__    = '2020/03/27'

class MeanDefender(Defender):

    # List of mean values.
    meanValues = None

    # Number of values included in mean.
    meanCount = 0

    # Treshhold in percent.
    treshhold = 0

    def __init__(self, config):
        super().__init__(config)

        try:
            self.treshhold = float(config['treshhold'])
        except:
            pass

    def learn(self, processValues):
        """ Calculate mean for each variable. """

        if self.meanValues is None:
            self.meanValues = [0] * len(processValues)

        for i in range(len(processValues)):
            v = processValues[i]

            if self.meanCount == 0:
                self.meanValues[i] = v
            else:
                # Calculate incremental mean.
                self.meanValues[i] = self.meanValues[i] + ((v - self.meanValues[i] ) / self.meanCount)
        self.meanCount += 1

    def detect(self, processValues):
        """ Check if the values stay within treshhold. """

        if self.meanValues is None:
            self.meanValues = [0] * len(processValues)

        for i in range(len(processValues)):
            v = processValues[i]
            m = self.meanValues[i]
            th = (m * self.treshhold) / 100.0
            minValue = m - th
            maxValue = m + th

            if v < minValue:
                self.attackDetected("Value out of treshhold.")
            if v > maxValue:
                self.attackDetected("Value out of treshhold.")
