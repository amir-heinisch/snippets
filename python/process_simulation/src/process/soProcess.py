"""

This file implements a process simulation abstraction via a shared object.

"""

from process.process import Process

import sys
import logging
from ctypes import CDLL

__author__  = 'Amir Heinisch'
__email__   = 'mail@amir-heinisch.de'
__date__    = '2019/11/15'

class SoProcess(Process):

    # Holds the shared object.
    simulation = None

    def __init__(self, config):
        """ This method loads the shared object. """

        try:
            path = config['processSimulationPath']
            if not path:
                logging.error("Simulation path must be given!")
                sys.exit(1)

            self.simulation = CDLL(path)
        except:
            logging.error("Can not load shared object...")
            sys.exit(1)
