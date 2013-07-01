#!/usr/bin/env python

# This one fails, because there is a module after the outboxless module.

from I3Tray import *
tray = I3Tray()
from icecube.icetray import I3Module





class NoOutboxModule(I3Module):
    def __init__(self, context):
        I3Module.__init__(self, context)

    def Configure(self):
        pass

    def Physics(self, frame):
        self.PushFrame(frame)

tray.AddModule("ManyStreamsSource","bottomless")
tray.AddModule(NoOutboxModule,"no_outbox")
tray.AddModule("Dump", "dump")

try:
    tray.Execute(10)
    assert False, "That should have thrown"
except RuntimeError as e:
    print("Okay, caught:", e)
    print("thrown as expected.")

