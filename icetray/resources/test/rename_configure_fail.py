#!/usr/bin/env python
import sys
from I3Tray import I3Tray

from icecube import icetray 

tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource")

tray.AddModule("Rename", Keys = ["here"])

try:
    tray.Execute(1)
except RuntimeError:
    pass
else:
    raise Exception("This should have failed")

