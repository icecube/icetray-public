#!/usr/bin/env python3
from icecube.icetray import I3Tray


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

