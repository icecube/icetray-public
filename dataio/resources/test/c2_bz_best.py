#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from icecube.icetray import I3Tray
from icecube import icetray, dataclasses, dataio

from os.path import expandvars

import os
import sys

tray = I3Tray()

tray.AddModule("I3Reader","reader",
               Filename =  expandvars("pass1_default.i3.bz2"))

tray.AddModule("Dump","dump")

tray.AddModule("I3Writer","writer",
               filename =  expandvars("pass1_best.i3.bz2"),
               CompressionLevel =  9)



tray.Execute()

