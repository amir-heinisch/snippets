"""
    This file just contains some helpers.
"""

__author__  = 'Amir Heinisch'
__email__   = 'mail@amir-heinisch.de'
__date__    = '2019/11/15'
__version__ = '1.0'

def toLower(str):
    """ This method just turns the first char of a word to lowercase. """
    return str[:1].lower() + str[1:] if str else ''

def sanitizeClass(str):
    return str

def commonPrefix(a, b):
    i = 0
    while i < len(a) and i < len(b) and a[i] == b[i]:
        i += 1
    return i

def eachWithPrefix(l):
    p = ''
    for x in l:
        yield commonPrefix(p, x), x
    p = x
