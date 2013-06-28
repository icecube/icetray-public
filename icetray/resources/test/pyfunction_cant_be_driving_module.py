#!/usr/bin/env python
#
# Sample i3module in python 
#

from icecube.icetray import *
from I3Tray import *

tray = I3Tray()

def pyfunction(frame):
    pass

tray.AddModule(pyfunction, "pyfunction") 

# print em
tray.AddModule("Dump","Dump")
# throw em out
tray.AddModule("TrashCan","TrashCan")

# do it 5 times.
try:
    tray.Execute()
    assert False, "that should have claimed that pyfunctions can't be driving modules"
except:
    print("Threw as expected")
