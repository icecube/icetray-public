#!/usr/bin/env python
#
#  $Id$
#
from I3Tray import *

from os.path import expandvars

import os
import sys

load("libdataclasses")
load("libphys-services")
load("libdataio")

tray = I3Tray()

# by default the reader will log_fatal if it can't deserialize something
tray.AddService("I3ReaderServiceFactory","readerfactory")(
    ("Filename", expandvars("hasmutineer.i3.gz"))
    )

tray.AddModule("I3Muxer","muxme")


# this guy actually does the get and causes the error
tray.AddModule("Get", "getter")(
    ("Keys", ["mutineer"])
    )

tray.AddModule("Dump","dump")

tray.AddModule("TrashCan", "the can");

try:
    tray.Execute()
    tray.Finish()

except:
    sys.exit(0)
else:
    print "***\n***\n*** Failure! Script didn't throw as it should have.\n***\n***\n***\n"
    sys.exit(1) # ought to die, shouldn't get here
