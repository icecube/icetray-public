#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import os
import unittest
import numpy as np

from icecube import dataclasses, dataio, icetray, phys_services
from icecube.dataclasses import I3mDOMLaunch, I3DEggLaunch, UpgradeLCFlags
from icecube.icetray import I3Tray, I3Units, OMKey

class ProduceHits(icetray.I3Module):
    frame_counter = 0
    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        self.AddOutBox('OutBox')

    def Configure(self):
        pass

    def DAQ(self, frame):
        mdom_launchmap = dataclasses.I3mDOMLaunchSeriesMap()
        degg_launchmap = dataclasses.I3DEggLaunchSeriesMap()

        # First case: hits are all on different strings, so no coincidences
        if self.frame_counter == 0:
            for string in range(10):
                omkey = icetray.OMKey(string, 1, 0)
                launch = I3mDOMLaunch()
                launch.time = 0
                mdom_launchmap[omkey] = [launch,]

        # Second case: Hits on a single pmt. No coincidences
        elif self.frame_counter == 1:
            omkey = icetray.OMKey(100, 50, 0)
            mdom_launchmap[omkey] = []
            for time in range(0, 50, 10):
                launch = I3mDOMLaunch()
                launch.time = time
                mdom_launchmap[omkey].append(launch)

        # Third case: Hits on a single module. Coincidences!
        # We're going to test with an intra-module window of
        # 100 ns. For that, we expect the first two to be
        # coincident, but the last two aren't.
        elif self.frame_counter == 2:
            for i, omkey in enumerate([OMKey(100, 1, 0),
                                       OMKey(100, 1, 4),
                                       OMKey(100, 1, 8),
                                       OMKey(100, 1, 16)]):
                launch = I3mDOMLaunch()
                launch.time = 100**i
                mdom_launchmap[omkey] = [launch,]

        # Fourth case: Hits on a multiple nearby mdoms. Coincidences!
        # For a span of 4, the first three will all be coincident.
        elif self.frame_counter == 3:
            for i, omkey in enumerate([OMKey(100, 1, 0),
                                       OMKey(100, 2, 2),
                                       OMKey(100, 4, 4),
                                       OMKey(100, 16, 16)]):
                launch = I3mDOMLaunch()
                launch.time = i
                mdom_launchmap[omkey] = [launch,]

        # Fifth case: Mix of mdoms and deggs. Coincidences!
        # For a span of 4, the first three will all be coincident.
        elif self.frame_counter == 4:
            for i, omkey in enumerate([OMKey(100, 1, 0),
                                       OMKey(100, 2, 2),
                                       OMKey(100, 4, 4),
                                       OMKey(100, 16, 16)]):
                if i in [0, 1, 4]:
                    launch = I3mDOMLaunch()
                    launch.time = i
                    mdom_launchmap[omkey] = [launch,]
                else:
                    launch = I3DEggLaunch()
                    launch.time = i
                    degg_launchmap[omkey] = [launch,]

        # Sixth case: Mix of mdoms and deggs. Coincidences!
        # For a span of 4, the first three will all be coincident.
        elif self.frame_counter == 5:
            for i, omkey in enumerate([OMKey(100, 1, 0),
                                       OMKey(100, 2, 2),
                                       OMKey(100, 4, 4),
                                       OMKey(100, 16, 16)]):
                if i in [0, 4]:
                    launch = I3mDOMLaunch()
                    launch.time = i
                    mdom_launchmap[omkey] = [launch,]
                else:
                    launch = I3DEggLaunch()
                    launch.time = i
                    degg_launchmap[omkey] = [launch,]

            self.RequestSuspension()

        frame['I3mDOMLaunchSeriesMap'] = mdom_launchmap
        frame['I3DEggLaunchSeriesMap'] = degg_launchmap
        frame['FrameCounter'] = icetray.I3Int(self.frame_counter)
        self.frame_counter += 1
        self.PushFrame(frame)


class I3MultiConcifyTest(unittest.TestCase):
    def test_I3MultiCoincify(self):
        mdommap = self.frame['I3mDOMLaunchSeriesMap']
        deggmap = self.frame['I3DEggLaunchSeriesMap']
        frame_num = self.frame['FrameCounter'].value
        mdom_flags = {"SingleModuleLC":0, "MultiModuleLC":0, "DEggInclusiveLC":0, "DEggExclusiveLC":0}
        degg_flags = {"SingleModuleLC":0, "MultiModuleLC":0, "DEggInclusiveLC":0, "DEggExclusiveLC":0}
        for launches in mdommap.values():
            for launch in launches:
                mdom_flags["SingleModuleLC"]  += bool(launch.lc_flags & UpgradeLCFlags.SingleModuleLC)
                mdom_flags["MultiModuleLC"]   += bool(launch.lc_flags & UpgradeLCFlags.MultiModuleLC)
                mdom_flags["DEggInclusiveLC"] += bool(launch.lc_flags & UpgradeLCFlags.DEggInclusiveLC)
                mdom_flags["DEggExclusiveLC"] += bool(launch.lc_flags & UpgradeLCFlags.DEggExclusiveLC)
        for launches in deggmap.values():
            for launch in launches:
                degg_flags["SingleModuleLC"]  += bool(launch.lc_flags & UpgradeLCFlags.SingleModuleLC)
                degg_flags["MultiModuleLC"]   += bool(launch.lc_flags & UpgradeLCFlags.MultiModuleLC)
                degg_flags["DEggInclusiveLC"] += bool(launch.lc_flags & UpgradeLCFlags.DEggInclusiveLC)
                degg_flags["DEggExclusiveLC"] += bool(launch.lc_flags & UpgradeLCFlags.DEggExclusiveLC)

        msg = (f"MultiPMTCoincify's results are {mdom_flags} for mDOMs and {degg_flags} for DEggs")
        print(msg)
        
        # mDOMs should never be DEgg Exclusive
        self.assertEqual(mdom_flags["DEggExclusiveLC"], 0)

        # First and second cases: no coincidences
        if frame_num in [0, 1]:
            msg = ("MultiPMTCoincify's testing failed on the first or second test"
                   f" with results {mdom_flags} for mDOMs and {degg_flags} for DEggs"
                   " when there should be 0 LC hits.")
            self.assertEqual(mdom_flags["SingleModuleLC"], 0, msg)
            self.assertEqual(mdom_flags["MultiModuleLC"],  0, msg)
            self.assertEqual(mdom_flags["DEggInclusiveLC"], 0, msg)
            self.assertEqual(mdom_flags["DEggExclusiveLC"], 0, msg)

            self.assertEqual(degg_flags["SingleModuleLC"], 0, msg)
            self.assertEqual(degg_flags["MultiModuleLC"],  0, msg)
            self.assertEqual(degg_flags["DEggInclusiveLC"], 0, msg)
            self.assertEqual(degg_flags["DEggExclusiveLC"], 0, msg)
        # Third case: only mdom single module coincidences
        elif frame_num == 2:
            msg = ("MultiPMTCoincify's testing failed on the third test"
                   f" with results {mdom_flags} for mDOMs and {degg_flags} for DEggs"
                   " when there should be only 2 single-module LC mdom hits.")
            self.assertEqual(mdom_flags["SingleModuleLC"], 2, msg)
            self.assertEqual(mdom_flags["MultiModuleLC"],  0, msg)
            self.assertEqual(mdom_flags["DEggInclusiveLC"], 0, msg)
            self.assertEqual(mdom_flags["DEggExclusiveLC"], 0, msg)

            self.assertEqual(degg_flags["SingleModuleLC"], 0, msg)
            self.assertEqual(degg_flags["MultiModuleLC"],  0, msg)
            self.assertEqual(degg_flags["DEggInclusiveLC"], 0, msg)
            self.assertEqual(degg_flags["DEggExclusiveLC"], 0, msg)
        # Fourth case: Multi-module coincidences across mdoms
        elif frame_num == 3:
            msg = ("MultiPMTCoincify's testing failed on the fourth test"
                   f" with results {mdom_flags} for mDOMs and {degg_flags} for DEggs"
                   " when there should be only 3 multi-module LC mdom hits.")
            self.assertEqual(mdom_flags["SingleModuleLC"], 0, msg)
            self.assertEqual(mdom_flags["MultiModuleLC"],  3, msg)
            self.assertEqual(mdom_flags["DEggInclusiveLC"], 0, msg)
            self.assertEqual(mdom_flags["DEggExclusiveLC"], 0, msg)

            self.assertEqual(degg_flags["SingleModuleLC"], 0, msg)
            self.assertEqual(degg_flags["MultiModuleLC"],  0, msg)
            self.assertEqual(degg_flags["DEggInclusiveLC"], 0, msg)
            self.assertEqual(degg_flags["DEggExclusiveLC"], 0, msg)
        # Fifth case: Multi-module coincidences including one DEgg
        elif frame_num == 4:
            msg = ("MultiPMTCoincify's testing failed on the fifth test"
                   f" with results {mdom_flags} for mDOMs and {degg_flags} for DEggs"
                   " when there should be 2 mDOM LC hits and 1 DEgg LC hit.")
            self.assertEqual(mdom_flags["SingleModuleLC"], 0, msg)
            self.assertEqual(mdom_flags["MultiModuleLC"],  2, msg)
            self.assertEqual(mdom_flags["DEggInclusiveLC"], 1, msg)
            self.assertEqual(mdom_flags["DEggExclusiveLC"], 0, msg)

            self.assertEqual(degg_flags["SingleModuleLC"], 0, msg)
            self.assertEqual(degg_flags["MultiModuleLC"],  1, msg)
            self.assertEqual(degg_flags["DEggInclusiveLC"], 1, msg)
            self.assertEqual(degg_flags["DEggExclusiveLC"], 0, msg)
        # Sixth case: Multi-module coincidences including two DEggs
        elif frame_num == 4:
            msg = ("MultiPMTCoincify's testing failed on the sixth test"
                   f" with results {mdom_flags} for mDOMs and {degg_flags} for DEggs"
                   " when there should be 1 mDOM LC hit and 1 DEgg LC hits.")
            self.assertEqual(mdom_flags["SingleModuleLC"], 0, msg)
            self.assertEqual(mdom_flags["MultiModuleLC"],  1, msg)
            self.assertEqual(mdom_flags["DEggInclusiveLC"], 1, msg)
            self.assertEqual(mdom_flags["DEggExclusiveLC"], 0, msg)

            self.assertEqual(degg_flags["SingleModuleLC"], 0, msg)
            self.assertEqual(degg_flags["MultiModuleLC"],  2, msg)
            self.assertEqual(degg_flags["DEggInclusiveLC"], 2, msg)
            self.assertEqual(degg_flags["DEggExclusiveLC"], 2, msg)

# --------------------------------------
# Actually run the stuff for testing
# --------------------------------------
tray = I3Tray()
tray.AddModule("I3InfiniteSource","TheSource", Stream=icetray.I3Frame.DAQ)
tray.Add(ProduceHits)

from icecube import phys_services
tray.Add("MultiPMTCoincify",
         mDOMLaunchMapName = "I3mDOMLaunchSeriesMap",
         DEggLaunchMapName = "I3DEggLaunchSeriesMap",
         Span = 2,
         TimeWindowBetweenModules = 1 * icetray.I3Units.microsecond,
         TimeWindowBetweenPMTs = 100 * icetray.I3Units.nanosecond,
         LCType = (UpgradeLCFlags.SingleModuleLC | UpgradeLCFlags.MultiModuleLC
                   | UpgradeLCFlags.DEggInclusiveLC | UpgradeLCFlags.DEggExclusiveLC))

tray.Add("Dump")

tray.AddModule(icetray.I3TestModuleFactory(I3MultiConcifyTest),'testmodule',
               Streams = [icetray.I3Frame.DAQ,])
tray.Execute()

