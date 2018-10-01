#!/usr/bin/env python
#
# Test that icetray throws if you start the chain with a python function
# They can't be driving modules.  Meaning they can't emit frames and
# IceTray won't emmit for them.
#

from icecube.icetray import *
from I3Tray import *

tray = I3Tray()

def pyfunction(frame):
    pass

tray.AddModule(pyfunction) 
tray.AddModule("Dump")

try:
    tray.Execute()
except:
    pass
else:
    raise Exception("That should have claimed that pyfunctions can't be driving modules")
