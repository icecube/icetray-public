#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from icecube.icetray import I3Tray

from os.path import expandvars

import os
import sys
from glob import glob

from icecube import dataclasses
from icecube import dataio

tray = I3Tray()

tray.AddModule("I3Reader","reader")(
    ("Filename", "pass1.i3")
    )

tray.AddModule("Dump","dump")

tray.AddModule("I3Writer","gcdwriter")(
    ("filename", "split.gcd.i3"),
    ("streams", ["Geometry", "Calibration", "DetectorStatus"])
    )

tray.AddModule("I3MultiWriter","physwriter")(
    ("filename", "split.physics.%04u.i3"),
    ("streams", ["Physics", "DAQ"]),
    ("sizelimit", 10**5)
    )



tray.Execute()





