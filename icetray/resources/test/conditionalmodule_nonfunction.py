#!/usr/bin/env python
#
# Simplest possible icetray script.
#

from I3Tray import *
import sys
tray = I3Tray()

try:
    tray.Add("BottomlessSource")
    tray.Add("AddNulls",
             where = ['foo'],
             If = [1,2,3])
    tray.Add("Dump")    
    tray.Execute(3)
except Exception as e:
    print(e, "ok, at that threw as expected.")
    sys.exit(0)
else:
    print("ERROR Shouldn't get here, that should have thrown.")
    sys.exit(1)




