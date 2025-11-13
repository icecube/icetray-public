#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2025 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import os

from icecube import icetray, dataio, phys_services
from icecube.icetray import I3Tray
from icecube.tableio import I3TableWriter
from icecube.tableio import I3SQLiteTableService
from icecube.dataclasses import I3Particle, I3EventHeader


TEST_DATA = os.path.expandvars("$I3_TESTDATA")

tray = I3Tray()

tray.AddModule("I3Reader",
    Filename = os.path.join(TEST_DATA, "sim/2023_NuGen_NuMu_22644_sample.i3.zst"),
)

out_path = os.path.join(os.getcwd(), "2023_NuGen_NuMu_22644_sample.db")
if os.path.isfile(out_path):
    os.unlink(out_path)
print(f"Writing test-database to {out_path} ...")

writer = I3SQLiteTableService(path = out_path)
tray.AddModule(I3TableWriter,
               "writer",
               TableService = writer,
               Keys = ["I3EventHeader", "I3MCTree"],
               SubEventStreams = ["InIceSplit", "Final"],
)

tray.Execute()
tray.Finish()

os.unlink(out_path)


