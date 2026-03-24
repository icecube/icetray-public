#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import os
import unittest
import numpy as np

from icecube import dataclasses, dataio, icetray, phys_services
from icecube.dataclasses import I3mDOMLaunch, I3DEggLaunch, I3XDOMHit, UpgradeLCFlags
from icecube.icetray import I3Tray, I3Units, OMKey

#--------------------------------------
# Implement testing for the MultiPMTCoincify module.
#
# An important note: This module requires a GCD file to be present. In this
# test, we're using $I3_TESTDATA/GCD/GeoCalibDetectorStatus_ICUpgrade.v55.mixed_mergedGeo.V5.i3.bz2.
# This is an older Upgrade GCD file, but it has OMTypes set, which is the
# only part that matters here. In order to test this code, we need to know
# the order of modules at some point in the detector. Here, we'll be using
# string 88, which has this structure in the region of interest:
#   ModuleKey(88, 41) # mdom
#   ModuleKey(88, 42) # DEgg
#   ModuleKey(88, 43) # DEgg
#   ModuleKey(88, 44) # mdom
#   ModuleKey(88, 45) # mdom
#   ModuleKey(88, 46) # DEgg
#   ModuleKey(88, 47) # mdom
#   ModuleKey(88, 48) # mdom
#   ModuleKey(88, 49) # DEgg
#   ModuleKey(88, 50) # mdom
# We must be aware of the order of these modules when designing the tests.
#--------------------------------------
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
        xdom_hitmap    = dataclasses.I3XDOMHitSeriesMap()

        # First case: hits are all on different strings, so no coincidences
        if self.frame_counter == 0:
            for string in range(87, 92):
                omkey = icetray.OMKey(string, 100, 0)
                launch = I3mDOMLaunch()
                launch.time = 0
                mdom_launchmap[omkey] = [launch,]

        # Second case: Hits on a single pmt. No coincidences
        elif self.frame_counter == 1:
            omkey = icetray.OMKey(88, 41, 0) # mdom
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
            for i, omkey in enumerate([OMKey(88, 41, 0), # mdom
                                       OMKey(88, 41, 4), # mdom
                                       OMKey(88, 41, 8), # mdom
                                       OMKey(88, 41, 16)]): # mdom
                launch = I3mDOMLaunch()
                launch.time = 100**i
                mdom_launchmap[omkey] = [launch,]

        # Fourth case: Hits on a multiple nearby mdoms. Coincidences!
        # For a span of 4, the first three will all be coincident.
        elif self.frame_counter == 3:
            for i, omkey in enumerate([OMKey(88, 41, 0), # mdom
                                       OMKey(88, 44, 2), # mdom
                                       OMKey(88, 45, 22), # mdom
                                       OMKey(88, 50, 16)]): # mdom
                launch = I3mDOMLaunch()
                launch.time = i
                mdom_launchmap[omkey] = [launch,]

        # Fifth case: Mix of mdoms and deggs. Coincidences!
        # For a span of 4, the first three will all be coincident.
        elif self.frame_counter == 4:
            for i, omkey in enumerate([OMKey(88, 41, 0), # mdom
                                       OMKey(88, 43, 1), # degg
                                       OMKey(88, 45, 4), # mdom
                                       OMKey(88, 50, 16)]):# mdom
                if i in [0, 2, 3]:
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
            for i, omkey in enumerate([OMKey(88, 41, 0), #mdom
                                       OMKey(88, 43, 0), #degg
                                       OMKey(88, 46, 1), #degg
                                       OMKey(88, 50, 16)]): #mdom
                if i in [0, 4]:
                    launch = I3mDOMLaunch()
                    launch.time = i
                    mdom_launchmap[omkey] = [launch,]
                else:
                    launch = I3DEggLaunch()
                    launch.time = i
                    degg_launchmap[omkey] = [launch,]
                    
        # Seventh case: Mix of mdoms and deggs, but with some in
        # I3XDOMHits from onboard pulse extractions. Coincidences!
        # For a span of 4, the first three will all be coincident.
        elif self.frame_counter == 6:
            launch = I3mDOMLaunch()
            launch.time = 0
            mdom_launchmap[OMKey(88, 41, 0)] = [launch,] #mdom
                        
            launch = I3DEggLaunch()
            launch.time = 1
            degg_launchmap[OMKey(88, 46, 1)] = [launch,]
            
            for i, omkey in enumerate([OMKey(88, 43, 0), #degg
                                       OMKey(88, 50, 16)]): #mdom
                hit = I3XDOMHit()
                hit.time = i
                xdom_hitmap[omkey] = [hit,]
            
            self.RequestSuspension()

        frame['I3mDOMLaunchSeriesMap'] = mdom_launchmap
        frame['I3DEggLaunchSeriesMap'] = degg_launchmap
        frame['I3XDOMHitSeriesMap']    = xdom_hitmap
        frame['FrameCounter'] = icetray.I3Int(self.frame_counter)
        self.frame_counter += 1
        self.PushFrame(frame)


class I3MultiConcifyTest(unittest.TestCase):
    def test_I3MultiCoincify(self):
        def extract(hitmap_name):
            hitmap = self.frame[hitmap_name]
            flags = {"SingleModuleLC":0, "MultiModuleLC":0, "DEggInclusiveLC":0, "DEggExclusiveLC":0}
            for hits in hitmap.values():
                for hit in hits:
                    flags["SingleModuleLC"]  += bool(hit.lc_flags & UpgradeLCFlags.SingleModuleLC)
                    flags["MultiModuleLC"]   += bool(hit.lc_flags & UpgradeLCFlags.MultiModuleLC)
                    flags["DEggInclusiveLC"] += bool(hit.lc_flags & UpgradeLCFlags.DEggInclusiveLC)
                    flags["DEggExclusiveLC"] += bool(hit.lc_flags & UpgradeLCFlags.DEggExclusiveLC)
            return flags
                
        mdom_flags = extract('I3mDOMLaunchSeriesMap')
        degg_flags = extract('I3DEggLaunchSeriesMap')
        xdom_flags = extract('I3XDOMHitSeriesMap')
        frame_num = self.frame['FrameCounter'].value

        # mDOMs should never be DEgg Exclusive
        self.assertEqual(mdom_flags["DEggExclusiveLC"], 0)

        def check_flags(observed, expected, msg):
            self.assertEqual(observed['SingleModuleLC'],  expected[0], msg)
            self.assertEqual(observed['MultiModuleLC'],   expected[1], msg)
            self.assertEqual(observed['DEggInclusiveLC'], expected[2], msg)
            self.assertEqual(observed['DEggExclusiveLC'], expected[3], msg)
            return
        
        # First and second cases: no coincidences
        if frame_num in [0, 1]:
            msg = ("MultiPMTCoincify's testing failed on the first or second test"
                   f" with results {mdom_flags} for mDOMs, {degg_flags} for DEggs,"
                   f" and {xdom_flags} for XDOMHits when there should be 0 LC hits.")
            check_flags(mdom_flags, [0, 0, 0, 0], msg)
            check_flags(degg_flags, [0, 0, 0, 0], msg)
            check_flags(xdom_flags, [0, 0, 0, 0], msg)

        # Third case: only mdom single module coincidences
        elif frame_num == 2:
            msg = ("MultiPMTCoincify's testing failed on the third test"
                   f" with results {mdom_flags} for mDOMs, {degg_flags} for DEggs,"
                   f" and {xdom_flags} for XDOMHits when there should be only 2 single-module LC mdom hits.")
            check_flags(mdom_flags, [2, 0, 0, 0], msg)
            check_flags(degg_flags, [0, 0, 0, 0], msg)
            check_flags(xdom_flags, [0, 0, 0, 0], msg)
            
        # Fourth case: Multi-module coincidences across mdoms
        elif frame_num == 3:
            msg = ("MultiPMTCoincify's testing failed on the fourth test"
                   f" with results {mdom_flags} for mDOMs, {degg_flags} for DEggs,"
                   f" and {xdom_flags} for XDOMHits when there should be only 3 multi-module LC mdom hits.")
            check_flags(mdom_flags, [0, 3, 0, 0], msg)
            check_flags(degg_flags, [0, 0, 0, 0], msg)
            check_flags(xdom_flags, [0, 0, 0, 0], msg)

        # Fifth case: Multi-module coincidences including one DEgg
        elif frame_num == 4:
            msg = ("MultiPMTCoincify's testing failed on the fifth test"
                   f" with results {mdom_flags} for mDOMs, {degg_flags} for DEggs,"
                   f" and {xdom_flags} for XDOMHits when there should be 2 mDOM LC hits and 1 DEgg LC hit.")
            check_flags(mdom_flags, [0, 2, 2, 0], msg)
            check_flags(degg_flags, [0, 1, 1, 0], msg)
            check_flags(xdom_flags, [0, 0, 0, 0], msg)
            
        # Sixth case: Multi-module coincidences including two DEggs
        elif frame_num == 5:
            msg = ("MultiPMTCoincify's testing failed on the sixth test"
                   f" with results {mdom_flags} for mDOMs, {degg_flags} for DEggs,"
                   f" and {xdom_flags} for XDOMHits when there should be 1 mDOM LC hit and 3 DEgg LC hits.")
            check_flags(mdom_flags, [0, 1, 1, 0], msg)
            check_flags(degg_flags, [0, 3, 3, 2], msg)
            check_flags(xdom_flags, [0, 0, 0, 0], msg)

        # Seventh case: Multi-module coincidences including two DEggs
        elif frame_num == 6:
            msg = ("MultiPMTCoincify's testing failed on the seventh test"
                   f" with results {mdom_flags} for mDOMs, {degg_flags} for DEggs,"
                   f" and {xdom_flags} for XDOMHits when there should be 1 mDOM LC hit, 1 DEgg LC hit, and"
                   f" 2 XDOM LC hits.")
            
            check_flags(mdom_flags, [0, 1, 1, 0], msg)
            check_flags(degg_flags, [0, 1, 1, 1], msg)
            check_flags(xdom_flags, [0, 2, 2, 1], msg)

# --------------------------------------
# Actually run the stuff for testing
# --------------------------------------
tray = I3Tray()
tray.AddModule("I3InfiniteSource","TheSource", Stream=icetray.I3Frame.DAQ,
               prefix = os.path.expandvars("$I3_TESTDATA/GCD/GeoCalibDetectorStatus_ICUpgrade.v55.mixed_mergedGeo.V5.i3.bz2"))
tray.Add(ProduceHits)

from icecube import phys_services
tray.Add("MultiPMTCoincify",
         mDOMLaunchMapName = "I3mDOMLaunchSeriesMap",
         DEggLaunchMapName = "I3DEggLaunchSeriesMap",
         xDOMHitMapName    = "I3XDOMHitSeriesMap", 
         Span = 4,
         TimeWindowBetweenModules = 1 * icetray.I3Units.microsecond,
         TimeWindowBetweenPMTs = 100 * icetray.I3Units.nanosecond,
         LCType = (UpgradeLCFlags.SingleModuleLC | UpgradeLCFlags.MultiModuleLC
                   | UpgradeLCFlags.DEggInclusiveLC | UpgradeLCFlags.DEggExclusiveLC))

tray.AddModule(icetray.I3TestModuleFactory(I3MultiConcifyTest),'testmodule',
               Streams = [icetray.I3Frame.DAQ,])
tray.Execute()

