#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2025 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import os
import shutil

from icecube import icetray, dataio, phys_services
from icecube.icetray import I3Tray
from icecube.tableio import I3TableWriter
from icecube.tableio import I3ParquetTableService
from icecube.dataclasses import I3Particle, I3EventHeader


TEST_DATA = os.path.expandvars("$I3_TESTDATA")

tray = I3Tray()

tray.AddModule("I3Reader",
    Filename = os.path.join(TEST_DATA, "sim/2023_NuGen_NuMu_22644_sample.i3.zst"),
)

out_folder = os.path.join(os.getcwd(), "2023_NuGen_NuMu_22644_sample")
if os.path.isdir(out_folder):
    shutil.rmtree(out_folder)
print(f"Writing test-parquet-files to {out_folder} ...")

writer = I3ParquetTableService(folder_path = out_folder, compression = "gzip")
tray.AddModule(I3TableWriter,
               "writer",
               TableService = writer,
               Keys = ["I3EventHeader", "I3MCTree"],
               SubEventStreams = ["InIceSplit", "Final"],
)

tray.Execute()
tray.Finish()

shutil.rmtree(out_folder)


