#!/usr/bin/env python

#
# This tray should complain that a module with an inbox is after a
#  module with no outbox.
#
from I3Tray import *
tray = I3Tray()
from icecube.icetray import I3Module


class DropModule(I3Module):
    def __init__(self, context):
        I3Module.__init__(self, context)

    def Configure(self):
        pass

    def Physics(self, frame):
        pass

# generate empty frames
tray.AddModule("ManyStreamsSource","src")
tray.AddModule("Dump","dump")

tray.AddModule(DropModule,"drop")

tray.AddModule("TrashCan","tc")

try:
    tray.Execute(15)
    assert False, "That shoudl have thrown"
except RuntimeError, e:
    print "good, caught", e
    print "okay."



