#!/usr/bin/env python
#
# Simplest possible icetray script.
#

from I3Tray import *
import sys
tray = I3Tray()

try:
    tray.AddModule("BottomlessSource", "bs")

    # generate empty frames
    tray.AddModule("AddNulls","an",
                   where = ['foo'],
                   If = [1,2,3])

    tray.AddModule("Dump", "dump")
    tray.AddModule("TrashCan", "tc")

    print("About to execute    ")
    # do it 5 times.
    tray.Execute(3)
except Exception as e:
    print(e, "ok, at that threw as expected.")
    sys.exit(0)
else:
    print("ERROR Shouldn't get here, that should have thrown.")
    sys.exit(1)




