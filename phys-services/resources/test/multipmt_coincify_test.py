#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import os
import unittest
import numpy as np

import I3Tray
from icecube import dataclasses, dataio, icetray, phys_services
from icecube.dataclasses import I3RecoPulse
from icecube.icetray import I3Units, OMKey

class ProducePulses(icetray.I3Module):
    frame_counter = 0
    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        self.AddOutBox('OutBox')

    def Configure(self):
        pass

    def DAQ(self, frame):
        pulsemap = dataclasses.I3RecoPulseSeriesMap()

        # First case: hits are all on different strings, so no coincidences
        if self.frame_counter == 0:
            for string in range(10):
                omkey = icetray.OMKey(string, 1, 0)
                pulse = I3RecoPulse()
                pulse.charge = 1
                pulse.time = 0
                pulsemap[omkey] = [pulse,]

        # Second case: Hits on a single pmt. No coincidences
        elif self.frame_counter == 1:
            omkey = icetray.OMKey(100, 50, 0)
            pulsemap[omkey] = []
            for time in range(0, 50, 10):
                pulse = I3RecoPulse()
                pulse.charge = 1
                pulse.time = time
                pulsemap[omkey].append(pulse)

        # Third case: Hits on a single module. Coincidences!
        # We're going to test with an intra-module window of
        # 100 ns. For that, we expect the first two to be
        # coincident, but the last two aren't.
        elif self.frame_counter == 2:
            for i, omkey in enumerate([OMKey(100, 1, 0),
                                       OMKey(100, 1, 4),
                                       OMKey(100, 1, 8),
                                       OMKey(100, 1, 16)]):
                pulse = I3RecoPulse()
                pulse.charge = 1
                pulse.time = 10**i - i
                pulsemap[omkey] = [pulse,]

        # Fourth case: Hits on a multiple nearby. Coincidences!
        # For a span of 4, the first three will all be coincident.
        else:
            for i, omkey in enumerate([OMKey(100, 1, 0),
                                       OMKey(100, 2, 2),
                                       OMKey(100, 4, 4),
                                       OMKey(100, 16, 16)]):
                pulse = I3RecoPulse()
                pulse.charge = 1
                pulse.time = i
                pulsemap[omkey] = [pulse,]
                self.RequestSuspension()

        frame['TestRecoPulseSeriesMap'] = pulsemap
        frame['FrameCounter'] = icetray.I3Int(self.frame_counter)
        self.frame_counter += 1
        self.PushFrame(frame)


class I3MultiConcifyTest(unittest.TestCase):
    def test_I3MultiCoincify(self):
        pulsemap = self.frame['TestRecoPulseSeriesMap']
        frame_num = self.frame['FrameCounter'].value
        n_hlc = 0
        for pulses in pulsemap.values():
            for p in pulses:
                n_hlc += p.flags & p.PulseFlags.LC

        nexpected = 0
        if frame_num == 0: nexpected = 0
        elif frame_num == 1: nexpected = 0
        elif frame_num == 2: nexpected = 3
        else: nexpected = 3
        self.assertEqual(n_hlc, nexpected)

# --------------------------------------
# Actually run the stuff for testing
# --------------------------------------
tray = I3Tray.I3Tray()
tray.AddModule("I3InfiniteSource","TheSource", Stream=icetray.I3Frame.DAQ)
tray.Add(ProducePulses)

from icecube import phys_services
tray.Add("MultiPMTCoincify",
         PulseMapName = "TestRecoPulseSeriesMap",
         Span = 2,
         TimeWindowBetweenModules = 1 * icetray.I3Units.microsecond,
         TimeWindowBetweenPMTs = 100 * icetray.I3Units.nanosecond)

tray.AddModule(icetray.I3TestModuleFactory(I3MultiConcifyTest),'testmodule',
               Streams = [icetray.I3Frame.DAQ,])
tray.Execute()

