#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from icecube import icetray, dataclasses, dataio
from icecube.icetray import I3Tray
import os

def write_empty_file(fname):
    """
    Run an I3Tray that emits no frames
    """

    tray = I3Tray()

    tray.Add("I3InfiniteSource")

    tray.Add("I3Writer", Streams=[icetray.I3Frame.DAQ, icetray.I3Frame.Physics],
        DropOrphanStreams=[icetray.I3Frame.DAQ],
        filename=fname)

    tray.Execute(1)


# all of these should exit cleanly
for ext in '.i3', '.i3.gz', '.i3.bz2':
    fname = "empty"+ext
    icetray.logging.log_notice("Writing empty file " + fname)
    write_empty_file(fname)
    os.unlink(fname)
