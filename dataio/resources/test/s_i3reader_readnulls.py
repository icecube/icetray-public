#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from icecube.icetray import I3Tray

from os.path import expandvars

import os
import sys

from icecube import dataclasses
from icecube import dataio

tray = I3Tray()

tray.AddModule("I3Reader","reader")(
    ("FileNameList", ["withnulls.i3"])
    )

tray.AddModule("Dump","dump")

tray.AddModule("FrameCheck", "checker")(
    ("Ensure_Physics_Has",
     ["I3Geometry", "I3Calibration", "I3DetectorStatus", "I3EventHeader"]),
    ("Ensure_DetectorStatus_Has",
     ["I3Geometry", "I3Calibration", "I3DetectorStatus"]),
    ("Ensure_Calibration_Has",
     ["I3Geometry", "I3Calibration"]),
    ("Ensure_Geometry_Has",
     ["I3Geometry"])
    )



tray.Execute()

