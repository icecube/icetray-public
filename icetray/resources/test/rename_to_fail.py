#!/usr/bin/env python
import sys
from I3Tray import I3Tray

from icecube import icetray 

def fill_frame(frame):
    frame['here'] = icetray.I3Bool(True)
    frame['there'] = icetray.I3Bool(True)

tray = I3Tray()
tray.Add("BottomlessSource")
tray.Add(fill_frame)
tray.Add("Rename", Keys = ["here", "there"])

try:
    tray.Execute(1)
except RuntimeError:
    pass
else:
    raise Exception("This should have failed")

