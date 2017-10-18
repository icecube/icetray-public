#!/usr/bin/env python

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
#  ... 'gcd' comes before 'physics'
file_list = glob("split.*.i3")

#  puts them in alpha/numeric order 
file_list.sort()

print(file_list)
tray.AddModule("I3Reader", FilenameList = file_list)

tray.AddModule("Dump")

# verify that 47 frames come through.  That's assumes only two files
# from test-data match the glob above.
tray.AddModule("CountFrames",
    DAQ = 10,
    Physics = 10,
    Calibration = 1,
    DetectorStatus = 1,
    Geometry = 1
    )

tray.Execute()

