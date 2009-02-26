#!/usr/bin/env python

from icecube import icetray, dataclasses, dataio, util
from I3Tray import *
import sys

tray = I3Tray()

tray.AddModule("BottomlessSource", "bs")
tray.AddModule("AddNulls", "N",
               Where = ['foo'],
               IcePickServiceKey = "key",
               If = lambda f: True)
tray.AddModule("Dump", "d")
tray.AddModule("TrashCan", "tc")

try:
    tray.Execute(1)
except Exception, e:
    print "ok, threw as expected"
    sys.exit(0)

assert False, "should have thrown"


