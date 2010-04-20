#!/usr/bin/env python

# This one *passes* though it looks like it shouldn't: You are allowed
#  to push to the generic 'outbox' (e.g. all outboxes) even if you
#  haven't added one.  What is an error is if there is a module after
#  this one.
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
        self.PushFrame(frame)

tray.AddModule("ManyStreamsSource","bottomless")
tray.AddModule("Dump", "dump")
tray.AddModule(NoOutboxModule,"no_outbox")

tray.Execute(10)


