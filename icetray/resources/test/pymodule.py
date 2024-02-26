#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

#
# Sample i3module in python
#

from icecube.icetray import I3Int, I3Module
from icecube.icetray import I3Tray

tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource")

class Mod(I3Module):
    def __init__(self, context):
        I3Module.__init__(self, context)
        self.AddParameter("Startfrom",
                          "what number to start from",
                          3)
        self.AddParameter("SumShouldBe",
                          "at the end",
                          5)

    def Configure(self):
        self.startfrom = self.GetParameter("Startfrom")
        self.sumshouldbe = self.GetParameter("SumShouldBe")

    def Physics(self, frame):
        i = I3Int(self.startfrom)
        frame.Put("it", i)
        self.startfrom+=1
        self.PushFrame(frame)

    def Finish(self):
        assert self.startfrom == self.sumshouldbe


tray.AddModule(Mod,
               Startfrom = 0,
               SumShouldBe = 6)

# print em
tray.AddModule("Dump")

# do it 5 times.
tray.Execute(6)
