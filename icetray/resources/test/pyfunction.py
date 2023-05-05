#!/usr/bin/env python3
#
# Sample i3module in python 
#

from icecube.icetray import I3Frame
from I3Tray import I3Tray

tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource")
tray.AddModule(lambda f: None)
tray.AddModule(lambda f: None, streams = [I3Frame.DAQ])
tray.Execute(5)

print("Okay. Exiting.")
