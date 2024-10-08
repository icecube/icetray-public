#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import sys
from icecube.icetray import I3Tray
from icecube import dataio

tray = I3Tray()

# by default the reader will log_fatal if it can't deserialize something
tray.AddModule("I3Reader", Filename = "hasmutineer.i3.gz")

tray.AddModule("FrameCheck", Ensure_Physics_Has = ["mutineer"])

tray.AddModule("DeleteUnregistered")

tray.AddModule("FrameCheck", Ensure_Physics_Hasnt = ["mutineer"])

tray.AddModule("Get")

tray.AddModule("Dump")

tray.AddModule("CountFrames", "count",
               DAQ = 10,
               Physics = 10,
               Geometry = 1,
               Calibration = 1,
               DetectorStatus = 1)

tray.Execute()


sys.exit(0)
