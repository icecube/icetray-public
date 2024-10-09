#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import sys

from icecube.icetray import I3Tray
from icecube import icetray, phys_services


class UseRandom(icetray.I3Module):
    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        self.AddParameter("I3RandomService", "the service", None)

    def Configure(self):
        self.rs = self.GetParameter("I3RandomService")

    def Physics(self, frame):
        if self.rs:
            # we don't actually need this
            self.rs.gaus(0, 1)

        self.PushFrame(frame)


try:
    tray = I3Tray()

    # generate empty frames
    tray.Add("BottomlessSource")
    tray.Add(UseRandom)
    tray.Add(UseRandom, I3RandomService=phys_services.I3MTRandomService(3141579))
    tray.Add(UseRandom, I3RandomService=None)
    tray.Add("ContextFreeServiceTestModule")
    tray.Execute(11)

except Exception as t:
    print(t)
    print("FAIL!!!")
    sys.exit(1)
else:
    # this should run without throwing
    sys.exit(0)
