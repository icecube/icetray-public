#!/usr/bin/env python

#
#  Must add outbox if you want to push
#
from I3Tray import *
tray = I3Tray()
from icecube.icetray import I3Module


class NoOutboxModule(I3Module):
    def __init__(self, context):
        I3Module.__init__(self, context)

    def Configure(self):
        pass

    def Physics(self, frame):
        self.PushFrame(frame, "OutBox")

# generate empty frames
tray.AddModule("BottomlessSource","bottomless")

# 2 dumps, both
tray.AddModule(NoOutboxModule,"do")
try:
    tray.Execute(1)
    assert False, "that should have thrown"
except RuntimeError as e:
    print("caught:", e)
    print("OK, that threw as we would have hoped.")



