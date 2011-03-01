#!/usr/bin/env python
#
# Sample i3module in python 
#

from icecube.icetray import *
from I3Tray import *

tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource","bottomless")

def make_counter(base):
    class ShouldCounter(base):
        def __init__(self, context):
            base.__init__(self, context)
            self.AddOutBox("OutBox")
            self.sdp = 0

        def Configure(self):
            pass

        def ShouldDoPhysics(self, frame):
            print base.__name__, "*** ShouldDoPhysics"
            self.sdp += 1
            return True

        def Physics(self, frame):
            print base.__name__, "*** sdp ==", self.sdp
            assert self.sdp == 1
            self.sdp = 0
            self.PushFrame(frame)
    return ShouldCounter
            

tray.AddModule(make_counter(I3Module), "modulecounter")
tray.AddModule(make_counter(I3ConditionalModule), "conditionalmodulecounter")

# throw em out
tray.AddModule("TrashCan","adios")

# do it 5 times.
tray.Execute(5)

# see ya.
tray.Finish()
