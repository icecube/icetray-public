#!/usr/bin/env python3

from icecube.icetray import I3Tray
import sys

tray = I3Tray()

tray.AddModule("BottomlessSource")
tray.AddModule("AddNulls", 
               Where = ['foo'],
               IcePickServiceKey = "key",
               If = lambda f: True)
tray.AddModule("Dump")

try:
    tray.Execute(1)
except Exception:
    print("ok, threw as expected")
    sys.exit(0)
else:
    sys.exit(1)


