#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import os
import sys

from icecube.icetray import I3Tray
from os.path import expandvars

from icecube import dataclasses
from icecube import phys_services
from icecube import dataio

tray = I3Tray()
tray.AddModule("I3Reader", Filename=expandvars("$I3_TESTDATA/dataio/olddata/string-21/MacOSX-libstdc++6-ppc.i3.gz"))
tray.Execute()

