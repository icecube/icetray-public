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

#
#  This takes a list of files.  You could use the python glob() function as well.
#


#  expand the I3_PORTS
globspec = expandvars("$I3_PORTS/test-data/string-21/*.i3.gz")

#  use 'glob' to convert the string with the 'star' in it to a list of real filenames
file_list = glob(globspec)

tray.AddModule("I3Reader","reader", FilenameList=file_list)

tray.AddModule("Dump","dump")

# verify that 47 frames come through.  That's assumes only two files
# from test-data match the glob above.
tray.AddModule("CountFrames", "count")(
    ("Physics", 44),
    ("Calibration", 2),
    ("DetectorStatus", 2),
    ("Geometry", 2)
    )

tray.AddModule("TrashCan", "the can");

tray.Execute()
tray.Finish()
