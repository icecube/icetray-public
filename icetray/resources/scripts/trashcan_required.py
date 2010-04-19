#!/usr/bin/env python

#
#  graph must be fully connected even if modules never use their outboxes
#

from I3Tray import *
from icecube.icetray import I3Module

class DoNothingModule(I3Module):
    def __init__(self, context):
        I3Module.__init__(self, context)
        self.AddOutBox('OutBox')

    def Process():
        pass

tray = I3Tray()

tray.AddModule("BottomlessSource","bottomless")
tray.AddModule(DoNothingModule, "nada")

try:
    tray.Execute(0)
    assert False, "that should have thrown"
except RuntimeError, e:
    print "caught:", e
    print "OK, that threw as we would have hoped."



