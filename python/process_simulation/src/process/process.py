"""

This file implements a genric api which we can use later to implement
different industrial process simulations like Tennessee Eastman.

"""

from abc import ABC, abstractmethod

__author__  = 'Amir Heinisch'
__email__   = 'mail@amir-heinisch.de'
__date__    = '2019/11/15'

class ProcessShutdown(Exception):
    """ This exception should be raised if the process shuts down gracefully. """

    def __init__(self, message):
        """
            Initialize self.

            Args:
                message (str): Reason of the process shutdown.
        """
        self.message = message

class ProcessBroke(Exception):
    """ This exception should be raised if the process broken in an unexpected way. """

    def __init__(self, message):
        """
            Initialize self.

            Args:
                message (str): Reason of the breakage.
        """
        self.message = message

class Process(ABC):

    # Count simulation rounds.
    round = 0

    # Holds the names of the process values.
    stateNames = []

    # Holds the distributed control loops.
    controlLoops = []

    def __init__(self, config=None):
        """
            This method should initialize the process simulation.

            Args:
                config (dict): Configurations for the process.
        """
        pass

    @property
    def state(self):
        """ This function returns the current process state. """
        return self._state.copy()

    @state.setter
    def state(self, newState):
        """
            This function updates the process state.

            Args:
                newState (list): New process state to use.
        """
        for i in range(len(newState)):
            self._state[i] = newState[i]

    @abstractmethod
    def simulate(self):
        """
            This method just increases the round counter.
            Overwrite it to do the real simulation stuff.
            NOTE! Do not forget to call this one or increase
            the round by yourself!
        """
        self.round += 1

    def runControlLoop(self, nr=0, state=None):
        """
            This method executes the plc control loop to calculate the update
            values for the next interval.
            NOTE! The attacker can run this with a given state and just
            manipulate the outcome before passing it to the update method.

            Args:
                state (list): Current process state to run the plcs on.
        """
        processState = state
        if nr in self.controlLoops:
            processState = self.controlLoops[nr](processState)
        else:
            for loop in self.controlLoops:
                processState = loop(processState)
        return processState
