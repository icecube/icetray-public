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

tray.AddModule("AddNulls", "stuff")(
    ("where", ["foo", "bar", "baz"])
    )
 
tray.AddModule("CountFrames", "count")(
    ("Physics", 11)
    )

# print em
tray.AddModule("Dump","dump")

# check whether frame element 'foo' exists (it shouldn't)
tray.AddModule("FrameCheck","checker")(
    ("ensure_physics_has", ["foo", "bar", "baz"]),
    )

tray.Execute(11)
