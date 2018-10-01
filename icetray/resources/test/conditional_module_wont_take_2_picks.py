#!/usr/bin/env python

from I3Tray import I3Tray
from icecube import dataio
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
except:
    print("ok, threw as expected")
    sys.exit(0)
else:
    sys.exit(1)


