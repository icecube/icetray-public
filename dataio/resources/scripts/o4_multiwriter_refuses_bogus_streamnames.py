#!/usr/bin/env python
#
#  use the standard messy monolith reader to get some data going.
#
from I3Tray import *

from os.path import expandvars

import os
import sys
from glob import glob

load("libdataclasses")
load("libphys-services")
load("libdataio")

tray = I3Tray()

tray.AddModule("I3MultiWriter","physwriter")(
    ("filename", "split.physics.%04u.i3"),
    ("streams", ["FIZZICKS"]),
    ("sizelimit", 10**5)
    )

tray.AddModule("TrashCan", "the can");

try:
    tray.Execute()
    tray.Finish()

    print "egh, module should have refused that parameter"
    sys.exit(1)
except:
    sys.exit(0)
    


