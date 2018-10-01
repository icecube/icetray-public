#!/usr/bin/env python
#
# Simplest possible icetray script.
#

from I3Tray import I3Tray
tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource")

# print em
tray.AddModule("Dump")

# do it 5 times.
tray.Execute(5)

tray.PrintUsage()
