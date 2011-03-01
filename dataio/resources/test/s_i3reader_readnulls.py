#!/usr/bin/env python
#
#  use the standard messy monolith reader to get some data going.
#
from I3Tray import *

from os.path import expandvars

import os
import sys

load("libdataclasses")
load("libdataio")

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

tray.AddModule("TrashCan", "the can");

tray.Execute()
tray.Finish()
