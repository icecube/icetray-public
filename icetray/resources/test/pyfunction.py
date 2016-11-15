#!/usr/bin/env python
#
# Sample i3module in python 
#

from icecube.icetray import *
from I3Tray import *

tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource")
tray.AddModule(lambda f: None)
tray.AddModule(lambda f: None, streams = [icetray.I3Frame.DAQ])
tray.Execute(5)

print("Okay. Exiting.")
