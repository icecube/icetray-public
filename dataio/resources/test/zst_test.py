#!/usr/bin/env python

from I3Tray import *

from os.path import expandvars

import os
import sys

from icecube import dataclasses 
from icecube import dataio


# Test standard compression (level = 4)
tray = I3Tray()
tray.AddModule("I3Reader","reader",
               Filename =  "pass1.i3")
tray.AddModule("Dump","dump")
tray.AddModule("CountFrames", DAQ=10, Physics=10,
               Calibration=1, DetectorStatus=1, Geometry=1)
tray.AddModule("I3Writer","writer",
               filename =  "pass1_default.i3.zst")
tray.Execute()

# Test best (level = 22)
tray = I3Tray()
tray.AddModule("I3Reader","reader",
               Filename =  "pass1_default.i3.zst")
tray.AddModule("Dump","dump")
tray.AddModule("CountFrames", DAQ=10, Physics=10,
               Calibration=1, DetectorStatus=1, Geometry=1)
tray.AddModule("I3Writer","writer",
               CompressionLevel = 22,
               filename = "pass1_best.i3.zst")
tray.Execute()

# Test fastest (level = 1)
tray = I3Tray()
tray.AddModule("I3Reader","reader",
               Filename =  "pass1_best.i3.zst")
tray.AddModule("Dump","dump")
tray.AddModule("CountFrames", DAQ=10, Physics=10,
               Calibration=1, DetectorStatus=1, Geometry=1)
tray.AddModule("I3Writer","writer",
               CompressionLevel = 1,
               filename = "pass1_fastest.i3.zst")
tray.Execute()

tray = I3Tray()
tray.AddModule("I3Reader","reader",
               Filename =  "pass1_fastest.i3.zst")
tray.AddModule("Dump","dump")
tray.AddModule("CountFrames", DAQ=10, Physics=10,
               Calibration=1, DetectorStatus=1, Geometry=1)
tray.Execute()