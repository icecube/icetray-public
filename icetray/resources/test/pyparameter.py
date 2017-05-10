#!/usr/bin/env python
#
# Sample i3module in python 
#

from I3Tray import I3Tray
from icecube import icetray
from icecube import dataclasses

class GetI3Int(icetray.I3Module):
    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        self.AddParameter("obj", "Python wrapped C++ class", None)

    def Configure(self):
        self.parameter = self.GetParameter("obj")


tray = I3Tray()

i3int = icetray.I3Int(777)

# generate empty frames
tray.AddModule(GetI3Int, obj=i3int)
tray.Execute(1)

