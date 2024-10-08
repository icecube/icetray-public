#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import sys
from icecube.icetray import I3Tray
from icecube import dataio

tray = I3Tray()

# by default the reader will log_fatal if it can't deserialize something
tray.AddModule("I3Reader","reader",
               Filename = "hasmutineer.i3.gz")

tray.AddModule("FrameCheck", "check1",
               Ensure_Physics_Has = ["mutineer"])

tray.AddModule("Get", "getter")

tray.AddModule("Dump","dump")



try:
    tray.Execute()
except:
    print("It threw as expected")
    sys.exit(0)
else:
    print("That should have thrown")
    sys.exit(1)



