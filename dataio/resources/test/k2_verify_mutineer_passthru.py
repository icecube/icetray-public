#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

#
#  verify that the previous script was able to write a file containing
#  objects that it didn't have serialization code for
#
from icecube.icetray import I3Tray

from os.path import expandvars

import os
import sys

from icecube import dataclasses
from icecube import phys_services
from icecube import dataio

tray = I3Tray()

# by default the reader will log_fatal if it can't deserialize something
tray.AddModule("I3Reader","reader", Filename="stillhasmutineer.i3.gz")

tray.AddModule("FrameCheck", "checker")(
    ("Ensure_Physics_Has", ["mutineer"])
    )

tray.AddModule("Dump","dump")


tray.Execute()
