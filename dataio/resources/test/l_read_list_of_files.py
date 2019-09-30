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

globspec = expandvars("$I3_TESTDATA/dataio/olddata/string-21/*.i3.gz")

#  use 'glob' to convert the string with the 'star' in it to a list of real filenames
file_list = glob(globspec)

tray.AddModule("I3Reader","reader", FilenameList=file_list)

tray.AddModule("Dump","dump")

# verify that 50 frames come through.
tray.AddModule("CountFrames", "count")(
    ("Physics", 44),
    ("Calibration", 2),
    ("DetectorStatus", 2),
    ("Geometry", 2)
    )



tray.Execute()

