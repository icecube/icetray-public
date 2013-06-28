#!/usr/bin/env python
#
# Sample i3module in python 
#

from icecube.icetray import *
from I3Tray import *

tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource","bottomless")

tray.AddModule(lambda f: None, "function")

tray.AddModule("TrashCan","TrashCan")

tray.Execute(5)

print("Okay. Exiting.")
