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
from icecube import phys_services
from icecube import dataio

tray = I3Tray()

tray.AddModule("I3Reader", "reader", Filename="pass1.i3")

tray.AddModule("Dump","dump")

tray.AddModule("I3MultiWriter","writer")(
    ("filename", "testmulti.%04u.i3"),
    ("sizelimit", 10**6), # 10^6 bytes
    ("MetadataStreams", [])
    )



tray.Execute()



