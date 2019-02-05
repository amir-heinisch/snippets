#!/usr/bin/env python3.7

"""
    This script schould notify me about the menu of the day.
"""

import requests
import xml.etree.ElementTree as elemt
from subprocess import call
import re

# NOTE! jarvis needs to be in PATH env var.

# Feed url.
xml_feed = "https://www.studentenwerk-saarland.de/_menuAtom/1"

# Set custom headers.
headers = {'user-agent': 'Mozilla'}

# Set timeout.
timeout = 10

try:
    # Create session and set retries.
    s = requests.Session()
    adapter = requests.adapters.HTTPAdapter(max_retries=3)
    s.mount('https://', adapter) # Every https request will use this adapter.
    # Run request.
    r = s.get(xml_feed, headers=headers, timeout=timeout, allow_redirects=False)
    # Raise exception if status is not 200.
    r.raise_for_status()
except: # ConnectionError, HTTPError, Timeout, SSLError
    call(['jarvis msg "Unable to load menu." "Mensa Bot"'], shell=True)    
finally:
    s.close()

# Parse xml result.
tree = elemt.fromstring(r.content)
ns = {'xmlns':'http://www.w3.org/2005/Atom'}
entry = tree.find('xmlns:entry', ns)
title = entry.find('xmlns:title', ns)
summary = entry.find('xmlns:summary', ns)

# Format menus.
sum = summary.text.replace('<br/>', '%0A').replace('<br />',
        '%0A').replace('-', '').replace('\n','')
sum = re.sub(r'\([^)]*\)', ' ', sum)
sum = re.sub(' +', ' ', sum)

# Split menus.
menus = sum.split('<b>')
# Remove empty string.
menus.pop(0)

# Send title message.
call(['jarvis msg "Hallo, hier ist der Speiseplan für ' + title.text + '" "Mensa Bot"'], shell=True)

# Send menus.
for menu in menus:
    print('<b>' + menu)
    call(['jarvis msg "%0A<b>' + menu + '" "Mensa Bot"'], shell=True)
