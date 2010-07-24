#!/usr/bin/env python

#
#  graph must be fully connected even if modules never use their outboxes
#

from I3Tray import *
from icecube.icetray import I3Module

tray = I3Tray()

tray.AddModule("BottomlessSource","bottomless")
tray.AddModule("TrashCan", "tschuess")

try:
    tray.Finish()
    assert False, "that should have thrown"
except RuntimeError, e:
    print "caught:", e
    print "OK, that threw as we would have hoped."



