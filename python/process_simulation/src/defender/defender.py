"""
    The is the abstract defender base class.
    A defender can see all values at the beginning of 
    each simulation round and can try to detect an attack.
    It is also possible to first let the defender learn
    before running an attack.
"""

from abc import ABC, abstractmethod

__author__  = 'Amir Heinisch'
__email__   = 'mail@amir-heinisch.de'
__date__    = '2020/03/18'

class AttackDetected(Exception):
    """ This exception should be raised if the defender detects an attack. """

    def __init__(self, message):
        self.message = message

class Defender(ABC):

    # Nuber of rounds before start to defend.
    learningRounds = 0

    # Ingore the first few detections.
    ignore = 0

    def __init__(self, config=None):
        """ Init defender with given config """

        try:
            self.learningRounds = int(config['roundsToLearn'])
            self.ignore = int(config['ignore'])
        except:
            pass

    def attackDetected(self, msg="Attack detected"):
        if self.ignore > 0:
            self.ignore -= 1
        else:
            raise AttackDetected(msg)

    def learn(self, processValues):
        """ This method should implement a learning algorithm. """
        pass

    # TODO: make this abstract.
    def detect(self, processValues):
        """
            This method should implement the detection algorithm.
            TODO: maybe it makes sense to implement a more detect hooks
            which the attacker call after running the control loops or
            eaven after each control loop.
        """
        pass
