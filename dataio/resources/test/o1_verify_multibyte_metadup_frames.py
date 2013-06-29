#!/usr/bin/env python
#
#  use the standard messy monolith reader to get some data going.
#
from I3Tray import *

from os.path import expandvars

import os
import sys
from glob import glob

from icecube import dataclasses 
from icecube import phys_services 
from icecube import dataio 

tray = I3Tray()

#
#  This takes a list of files.  You could use the python glob() function as well.
#


#  use 'glob' to convert the string with the 'star' in it to a list of real filenames
file_list = glob("testmultiD.*.i3")

#  puts them in alpha/numeric order 
file_list.sort()

print(file_list)
tray.AddModule("I3Reader", "reader", FilenameList=file_list)

tray.AddModule("Dump","dump")

# verify that 47 frames come through.  That's assumes only two files
# from test-data match the glob above.
tray.AddModule("CountFrames", "count")(
    ("Physics", 10),
    ("Calibration", 1),
    ("DetectorStatus", 3), # One for each subfile
    ("Geometry", 1)
    )

tray.AddModule("TrashCan", "the can");

tray.Execute()
tray.Finish()

