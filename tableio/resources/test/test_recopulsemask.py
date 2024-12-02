#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

# Ensure that it is possible to book I3RecoPulseSeriesMapMasks without error.

import os
import tempfile

from icecube import dataclasses, dataio, icetray, tableio

tray = icetray.I3Tray()

tray.AddModule("I3InfiniteSource", "source", stream=icetray.I3Frame.Physics)

def fakeit(frame):
    header = dataclasses.I3EventHeader()
    frame['I3EventHeader'] = header
    pulsemap = dataclasses.I3RecoPulseSeriesMap()
    pulses = dataclasses.I3RecoPulseSeries()
    pulse = dataclasses.I3RecoPulse()
    pulses.append(pulse)

    pulsemap[icetray.OMKey(7,42)] = pulses
    pulsemap[icetray.OMKey(9,42)] = pulses
    frame['Pulses'] = pulsemap
    mask = dataclasses.I3RecoPulseSeriesMapMask(frame, 'Pulses')
    frame['PulseMask'] = mask

tray.AddModule(fakeit, 'fakeit')

dirname = tempfile.TemporaryDirectory(
    dir=os.environ['I3_BUILD'] + '/tableio',
    prefix='pulsemask_test.'
)
tabler = tableio.I3CSVTableService(dirname.name)

tray.AddModule(tableio.I3TableWriter, 'scribe',
    tableservice=tabler,
    keys=['PulseMask'],
    )

tray.Execute(1)
