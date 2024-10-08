#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from icecube.icetray import I3Tray
from icecube import icetray


class SimpleModule(icetray.I3Module):
    def __init__(self, context):
        icetray.I3Module.__init__(self, context)


@icetray.traysegment
def simple_segment(tray, name):
    tray.AddModule("Dump")


tray = I3Tray()
tray.AddModule("BottomlessSource")

for _foo in range(0, 10):
    tray.AddModule(SimpleModule)
    tray.AddModule("Dump")
    tray.AddModule(lambda frame: True)
    tray.AddSegment(simple_segment)

tray.Execute(10)
