#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from icecube.icetray import I3Tray

from os.path import expandvars

import os
import sys

from icecube import dataclasses
from icecube import phys_services
from icecube import dataio

tray = I3Tray()

tray.AddModule("I3Reader", "reader", Filename=expandvars("withnulls.i3"))

tray.AddModule("Dump","dump")



tray.Execute()

