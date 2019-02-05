#!/usr/bin/env python3.7

"""
    This script schould notify me about danger in germany.

    TODO:
        - save current/sent notifications
        - log notifications
        - only show local if wanted
        - only show wanted notifications
        - remove expired ones
"""

import requests
import xml.etree.ElementTree as elemt
from subprocess import call
import re

# NOTE! jarvis needs to be in PATH env var.

# BBK warnings.
bbk_url = "https://warnung.bund.de/bbk.mowas/gefahrendurchsagen.json"
# DWD warnings (wether).
dwd_url = "https://warnung.bund.de/bbk.dwd/unwetter.json"

# Set custom headers.
headers = {'user-agent': 'Mozilla'}

# Set timeout.
timeout = 10

try:
    # This script should run like every two minutes. So no retries needed.
    r_bbk = requests.get(bbk_url, headers=headers, timeout=timeout, allow_redirects=False)
    r_dwd = requests.get(dwd_url, headers=headers, timeout=timeout, allow_redirects=False)
    # Raise exception if status is not 200.
    r_bbk.raise_for_status()
    r_dwd.raise_for_status()
except: # ConnectionError, HTTPError, Timeout, SSLError
    call(['jarvis msg "Unable to load current warnings." "Danger Bot"'], shell=True)    

# Parse json result.
warnings_bbk = r_bbk.json()
warnings_dwd = r_dwd.json()

# Send warnings.
# TODO - only send if there are new warnings.
call(['jarvis msg "Hallo, hier sind alle aktuellen Warnungen für Deutschland." "Danger Bot"'], shell=True)

# Send warnings.
# TODO - only send new warnings.
for warning in warnings_bbk:
    headline = warning['info'][0]['headline']
    area = warning['info'][0]['area'][0]['areaDesc']
    area = area.replace('Landkreis/Stadt:', '').replace(' ', '').strip()
    call(['jarvis msg "%0A<b>' + area + ':</b>%0A' + headline + '" "Danger Bot"'], shell=True)
for warning in warnings_dwd:
    headline = warning['info'][0]['headline']
    area = warning['info'][0]['area'][0]['areaDesc']
    area = area.replace('Landkreis/Stadt:', '').replace(' ', '').strip()
    call(['jarvis msg "%0A<b>' + area + ':</b>%0A' + headline + '" "Danger Bot"'], shell=True)
