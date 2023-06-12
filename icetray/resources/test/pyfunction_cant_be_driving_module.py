#!/usr/bin/env python3
#
# Test that icetray throws if you start the chain with a python function
# They can't be driving modules.  Meaning they can't emit frames and
# IceTray won't emmit for them.
#

from icecube.icetray import I3Tray

tray = I3Tray()

def pyfunction(frame):
    pass

tray.AddModule(pyfunction) 
tray.AddModule("Dump")

try:
    tray.Execute()
except Exception:
    pass
else:
    raise Exception("That should have claimed that pyfunctions can't be driving modules")
