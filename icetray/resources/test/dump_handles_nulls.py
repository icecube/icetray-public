#!/usr/bin/env python3
#
# Checking that the Dump handles nulls w/o crashing
#
from icecube.icetray import I3Tray

tray = I3Tray()

# generate empty frames
tray.Add("BottomlessSource")
tray.Add("AddNulls", where=["here", "there", "everywhere", "tonsastuff"])

# verify they are there
tray.Add("FrameCheck", ensure_physics_has=["here", "there", "everywhere", "tonsastuff"])

# print em.   lots of nulls, dump shouldn't crash.
tray.Add("Dump")

tray.Execute(15)
