#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

#
#  addoutbox works if it is in Configure
#

from icecube.icetray import I3Tray
from icecube.icetray import I3Module


class M(I3Module):
    def __init__(self, context):
        I3Module.__init__(self, context)

    def Physics(self, frame):
        self.PushFrame(frame, "OutBox")


tray = I3Tray()
# generate empty frames
tray.AddModule("BottomlessSource")
# 2 dumps, both
tray.AddModule(M)
tray.Execute(1)
