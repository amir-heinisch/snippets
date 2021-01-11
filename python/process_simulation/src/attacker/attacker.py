"""
    The *Attacker* class is an abstract base class you can use to implement
    a new attacker. An attacker basically just instantiate a process object
    and runs the simulation by repeatedly starting a new iteration round.
    By design an attacker is able to manipulate and monitor all values of
    the running process simulation. So this allows us to easily implement
    attackers with different levels of power to cover all sort of attack vectors.
    The *PassiveAttacker* is the most simple subclass, which just runs the
    process and logs the values.
"""

from process.process import ProcessShutdown, ProcessBroke, Process
from defender.defender import AttackDetected, Defender
from utils import toLower

import importlib
import threading
import logging
import sys
from abc import ABC, abstractmethod

__author__  = 'Amir Heinisch'
__email__   = 'mail@amir-heinisch.de'
__date__    = '2020/01/06'

class Attacker(ABC):

    # Holds the process object.
    process = None

    # Indicate if we are attacking at the moment.
    attacking = False

    # Safe start time of current attack window.
    attackStartTime = None

    # Output file.
    outputFile = None

    # Defender.
    defender = None

    def __init__(self, config=None):
        """
            Instantiate an attacker with a given config.

            Args:
                config (dict): Configurations for the attacker.
        """

        try:
            self.outputFile = open(config['outputPath'], "w")
        except:
            logging.error("Can't open file to write at given path.")

    def initializeProcess(self, config):
        """
            This method instantiates a process object for the given configuration.

            Args:
                config (dict): Configurations for the process.
        """

        # Import process module first.
        processClass = config['class']
        processPackage = config['package']
        logging.debug('Try to use process ' + processClass + ' from module process' + processPackage + "." + toLower(processClass))
        processModule = importlib.import_module('process' + processPackage + "." + toLower(processClass), package='...process')
        # Get process class.
        process_ = getattr(processModule, processClass)
        # Instanciate process.
        self.process = process_(config)
        logging.debug('Successfully created process ' + processClass + ' from module process.' + toLower(processClass) + ' [' + str(self.process) + ']')

    def initializeDefender(self, config):
        """
            This method instantiates a defender object for the given configuration.

            Args:
                config (dict): Configurations for the defender.
        """

        # Import defender module.
        defenderClass = config['class']
        logging.debug('Try to use defender ' + defenderClass + ' from module defender.' + toLower(defenderClass))
        defenderModule = importlib.import_module('.' + toLower(defenderClass), package='defender')
        # Get defender class.
        defender_ = getattr(defenderModule, defenderClass)
        # Instanciate defender.
        self.defender = defender_(config)
        logging.debug('Successfully created defender ' + defenderClass + ' from module defender.' + toLower(defenderClass) + ' [' + str(self.defender) + ']')

    def startAttack(self):
        """
            This method starts the attack function if needed.
            It is also responsible to catch the exceptions if an attack
            was detected or the process broke somehow.
        """

        # Chill if already attacking.
        if self.attacking:
            return

        # We need a process to run attack on.
        if self.process is None:
            logging.error("Init process before run an attack!")
            sys.exit(1)

        try:
            self.attack()
            self.attacking = True
        except (AttackDetected, ProcessShutdown, ProcessBroke) as e:
            if hasattr(e, 'message'):
                msg = "-> " + e.message + " (Round " + str(self.process.round) + ")"
            else:
                msg = "-> Exception(" + e.__class__.__name__ + ") (Round " + str(self.process.round) + ")"
            self.output(msg)
            logging.info(msg)
        except KeyboardInterrupt as e:
            self.output("-> Keyboard interrupt received, stopping.. (Round " + str(self.process.round) + ")")

            raise e

    @abstractmethod
    def attack(self):
        """
            This method runs the attack. It needs to be overwritten by a
            subclass where the actual attack code should be implement.
        """
        pass

    def output(self, str):
        """
            This function should output the attack infos in the needed format.
            We just print it to file for ease now.
            If you want an other format just overwrite it.

            Args:
                str (str): String to print.
        """

        if self.outputFile is not None:
            self.outputFile.write(str)
