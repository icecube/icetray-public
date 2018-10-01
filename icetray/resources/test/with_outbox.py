#!/usr/bin/env python

#
#  AddOutBox works if in constructor
#

from I3Tray import *
tray = I3Tray()
from icecube.icetray import I3Module

class M(I3Module):
    def __init__(self, context):
        I3Module.__init__(self, context)

    def Physics(self, frame):
        self.PushFrame(frame, "OutBox")

tray.AddModule("BottomlessSource")
tray.AddModule(M)
tray.Execute(1)


