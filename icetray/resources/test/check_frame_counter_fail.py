#!/usr/bin/env python
#
# Checking that the driving time exists
#
import sys
from I3Tray import *

from icecube import icetray 

tray = I3Tray()

# generate empty frames
tray.Add("BottomlessSource")
tray.Add("AddNulls", where = ["foo", "bar", "baz"])    
tray.AddModule("CountFrames", Physics = 11)
tray.AddModule("Dump")
tray.AddModule("FrameCheck", ensure_physics_has = ["foo", "bar", "baz"])    

try:
    tray.Execute(7)  # wrong number of frames    
except:
    print("ok: failure as expected.")
    sys.exit(0)
else:
    sys.exit(1)

