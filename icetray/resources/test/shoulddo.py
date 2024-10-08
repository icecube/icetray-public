#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

#
# Sample i3module in python
#

from icecube.icetray import I3Module, I3ConditionalModule
from icecube.icetray import I3Tray

tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource","bottomless")

def make_counter(base):
    class ShouldCounter(base):
        def __init__(self, context):
            base.__init__(self, context)
            self.sdp = 0

        def ShouldDoPhysics(self, frame):
            print(base.__name__ + " *** ShouldDoPhysics")
            self.sdp += 1
            return True

        def Physics(self, frame):
            print("%s *** sdp == %d" % (base.__name__, self.sdp))
            assert self.sdp == 1
            self.sdp = 0
            self.PushFrame(frame)
    return ShouldCounter


tray.AddModule(make_counter(I3Module), "modulecounter")
tray.AddModule(make_counter(I3ConditionalModule), "conditionalmodulecounter")

# do it 5 times.
tray.Execute(5)

