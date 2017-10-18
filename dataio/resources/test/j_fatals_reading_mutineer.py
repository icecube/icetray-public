#!/usr/bin/env python
#
#  $Id: j_fatals_reading_mutineer.py 1636 2013-06-29 03:02:36Z nwhitehorn $
#
from I3Tray import *

from os.path import expandvars

import os
import sys

from icecube import dataclasses 
from icecube import phys_services 
from icecube import dataio 

tray = I3Tray()

# by default the reader will log_fatal if it can't deserialize something
tray.AddModule("I3Reader","reader", Filename=expandvars("hasmutineer.i3.gz"))

# this guy actually does the get and causes the error
tray.AddModule("Get", "getter")(
    ("Keys", ["mutineer"])
    )

tray.AddModule("Dump","dump")

try:
    tray.Execute()
    

except:
    sys.exit(0)
else:
    print("***\n***\n*** Failure! Script didn't throw as it should have.\n***\n***\n***\n")
    sys.exit(1) # ought to die, shouldn't get here
