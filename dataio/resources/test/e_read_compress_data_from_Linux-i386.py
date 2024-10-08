#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import os
import sys

from icecube.icetray import I3Tray
from os.path import expandvars
from icecube import icetray, dataclasses, phys_services, dataio

tray = I3Tray()

tray.AddModule("I3Reader", Filename = expandvars("$I3_TESTDATA/dataio/olddata/string-21/Linux-i386.i3.gz"))
tray.AddModule("I3Writer",
               filename = "pass1_fastest.i3.gz",
               CompressionLevel=1)
tray.Execute()

