#!/usr/bin/env python

#
#  addoutbox works if it is in Configure
#
from I3Tray import *
tray = I3Tray()
from icecube.icetray import I3Module


class M(I3Module):
    def __init__(self, context):
        I3Module.__init__(self, context)

    def Physics(self, frame):
        self.PushFrame(frame, "OutBox")

# generate empty frames
tray.AddModule("BottomlessSource")

# 2 dumps, both
tray.AddModule(M)

tray.Execute(1)
