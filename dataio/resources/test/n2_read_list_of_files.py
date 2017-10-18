#!/usr/bin/env python

from I3Tray import *

from os.path import expandvars

import os
import sys
from glob import glob

from icecube import dataclasses 
from icecube import dataio 

tray = I3Tray()

#  use 'glob' to convert the string with the 'star' in it to a list of real filenames
file_list = glob("testm.*.i3.gz")

#  puts them in alpha/numeric order 
file_list.sort()

print(file_list)
tray.AddModule("I3Reader", FilenameList = file_list)

tray.AddModule("Dump")

# verify that 23 frames come through.
tray.AddModule("CountFrames",
    DAQ = 10,
    Physics = 10,
    Calibration = 1,
    DetectorStatus = 1,
    Geometry = 1
    )

tray.Execute()


