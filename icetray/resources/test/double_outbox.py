#!/usr/bin/env python

#
#  Modules can't add the same outbox multiple times
#
from I3Tray import *
tray = I3Tray()
from icecube.icetray import I3Module


class DoubleOutboxModule(I3Module):
    def __init__(self, context):
        I3Module.__init__(self, context)
    def Configure(self):
        self.AddOutBox("box")
        self.AddOutBox("box")
    def Physics(self, frame):
        self.PushFrame(frame)

# generate empty frames
tray.AddModule("BottomlessSource","bottomless")

# 2 dumps, both
tray.AddModule(DoubleOutboxModule,"do")
try:
    tray.Execute(1)
    assert False, "that should have thrown"
except:
    print "okay, that threw as we would have hoped."



