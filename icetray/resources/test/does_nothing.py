#!/usr/bin/env python
#
# Simplest possible icetray script.
#

from I3Tray import *
tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource","bottomless")

# print em
tray.AddModule("Dump","dump")

# throw em out
tray.AddModule("TrashCan","adios")

# do it 5 times.
tray.Execute(5)

# see ya.
tray.Finish()

tray.PrintUsage()
