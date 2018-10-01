#!/usr/bin/env python

from icecube.icetray import *
from I3Tray import *

tray = I3Tray()


# can't convert that return value to bool or none
def f(frame):
    return { "three" : 3 }

try:
    tray.AddModule("BottomlessSource","bs")
    tray.AddModule(f,"shouldfail")
    tray.Execute()
    assert False, "that should have thrown"
except RuntimeError, e:
    print "As expected, caught '%s'" % e

                   

