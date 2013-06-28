#!/usr/bin/env python
#
# Checking that the driving time exists
#
import sys
from I3Tray import *

from icecube import icetray 

tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource","bottomless")

# print em
tray.AddModule("Dump","dump")

# check whether frame element 'foo' exists (it shouldn't)
tray.AddModule("FrameCheck","checker")(
    ("ensure_physics_hasnt", ["foo", "bar", "baz"]),
    ("ensure_physics_has", ["this-will-cause-error"])
    )

# throw em out
tray.AddModule("TrashCan","adios")

try:
    # This should fail
    tray.Execute(5)
    tray.Finish()
except:
    print("ok: failure as expected.")
    # ok it failed
    sys.exit(0)
else:
    sys.exit(1)

