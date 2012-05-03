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

tray.AddModule("AddNulls", "nulls")(
    ("where", ["src1", "src2"])
    )

# verify they are there
tray.AddModule("FrameCheck","checker")(
    ("ensure_physics_has", ["src1", "src2"]),
    ("ensure_physics_hasnt", ["dst1", "dst2"])
    )

tray.AddModule("Copy", "copy")(
    ("Keys", ["src1", "dst1", "src2", "dst2"])
    )

tray.AddModule("FrameCheck","checker2")(
    ("ensure_physics_has", ["src1", "dst1", "src2", "dst2"]),
    )

# print em.   lots of nulls, dump shouldn't crash.
tray.AddModule("Dump","dump")

# throw em out
tray.AddModule("TrashCan","adios")

tray.Execute(15)
tray.Finish()

