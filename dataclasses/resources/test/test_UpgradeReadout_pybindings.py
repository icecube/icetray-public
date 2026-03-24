#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2025 The IceTray Contributors
# SPDX-License-Identifier: BSD-2-Clause
#
# Date 2026-03-20
# Author mlarson
import os
import unittest

from icecube import dataio, dataclasses, icetray
from icecube.icetray import OMKey
from icecube.dataclasses import (I3DEggLaunch, I3DEggLaunchSeriesMap,
                                 I3mDOMLaunch, I3mDOMLaunchSeriesMap,
                                 I3XDOMHit,    I3XDOMHitSeriesMap,
                                 UpgradeReadout, UpgradeReadoutSeriesMap,
                                 UpgradeLCFlags)

class I3mDOMLaunchTest(unittest.TestCase):
    def setUp(self):
        # Build the mDOM map
        self.mdom_omkeys = [OMKey(88, 41, 0), OMKey(88, 44, 12)]
        self.mdom_map = I3mDOMLaunchSeriesMap()
        for omkey in self.mdom_omkeys:
            self.mdom_map[omkey] = []
            for i in range(0, 100, 10):
                launch = I3mDOMLaunch()
                launch.time = i
                launch.lc_flags = UpgradeLCFlags.SingleModuleLC
                self.mdom_map[omkey].append(launch)

        # Build the DEgg map
        self.degg_omkeys = [OMKey(88, 42, 0), OMKey(88, 43, 1)]
        self.degg_map = I3DEggLaunchSeriesMap()
        for omkey in self.degg_omkeys:
            self.degg_map[omkey] = []
            for i in range(0, 100, 10):
                launch = I3DEggLaunch()
                launch.time = i
                launch.lc_flags = UpgradeLCFlags.MultiModuleLC
                self.degg_map[omkey].append(launch)
        
        # And build an XDOMHit map
        # Note that this explicitly overlaps with the mDOM
        # and DEgg maps above so that we can test the merging.
        self.xdom_omkeys = [self.mdom_omkeys[0], self.degg_omkeys[0], OMKey(88, 49, 0)]
        self.xdom_map = I3XDOMHitSeriesMap()
        for omkey in self.xdom_omkeys:
            self.xdom_map[omkey] = []
            for i in range(5, 105, 10):
                hit = I3XDOMHit()
                hit.time = i
                hit.lc_flags = UpgradeLCFlags.MultiModuleLC
                self.xdom_map[omkey].append(hit)

        # Prepare an I3OMGeoMap by reading a test gcd file
        gcd_path = os.path.expandvars("$I3_TESTDATA/GCD/GeoCalibDetectorStatus_ICUpgrade.v55.mixed_mergedGeo.V5.i3.bz2")
        gcd = dataio.I3File(gcd_path)
        self.omgeoMap = None
        while self.omgeoMap is None:
            frame = gcd.pop_frame()
            if "I3Geometry" in frame:
                self.omgeoMap = frame["I3Geometry"].omgeo
        return

    def compare_hits(self, hitA, hitB):
        self.assertTrue(hitA.lc_flags == hitB.lc_flags)
        self.assertTrue(hitA.time == hitB.time)
        
    def test_mdom_single_converters(self):
        omkey = self.mdom_omkeys[0]
        mdom_launch = self.mdom_map[omkey][0]
        readout = UpgradeReadout(mdom_launch, self.omgeoMap[omkey].omtype)

        self.compare_hits(readout, mdom_launch)
        
        self.assertTrue(readout.is_I3mDOMLaunch())
        self.assertFalse(readout.is_I3DEggLaunch())
        self.assertFalse(readout.is_I3XDOMHit())

        self.assertIsNotNone(readout.get_I3mDOMLaunch())
        self.assertIsNone(readout.get_I3DEggLaunch())
        self.assertIsNone(readout.get_I3XDOMHit())

    def test_degg_single_converters(self):
        omkey = self.degg_omkeys[0]
        degg_launch = self.degg_map[omkey][0]
        readout = UpgradeReadout(degg_launch, self.omgeoMap[omkey].omtype)
        self.compare_hits(readout, degg_launch)

        self.assertFalse(readout.is_I3mDOMLaunch())
        self.assertTrue(readout.is_I3DEggLaunch())
        self.assertFalse(readout.is_I3XDOMHit())

        self.assertIsNone(readout.get_I3mDOMLaunch())
        self.assertIsNotNone(readout.get_I3DEggLaunch())
        self.assertIsNone(readout.get_I3XDOMHit())

    def test_xdomhit_single_converters(self):
        # I3XDOMHits can come from multiple OMTypes, so just test
        # all three hit modules here.
        for omkey, xdom_hits in self.xdom_map.items():
            xdom_hit = xdom_hits[0]
            readout = UpgradeReadout(xdom_hit, self.omgeoMap[omkey].omtype)
            self.compare_hits(readout, xdom_hit)

            self.assertFalse(readout.is_I3mDOMLaunch())
            self.assertFalse(readout.is_I3DEggLaunch())
            self.assertTrue(readout.is_I3XDOMHit())
            
            self.assertIsNone(readout.get_I3mDOMLaunch())
            self.assertIsNone(readout.get_I3DEggLaunch())
            self.assertIsNotNone(readout.get_I3XDOMHit())
        return

    def test_map_constructors(self):
        readout_map = UpgradeReadoutSeriesMap(self.degg_map, self.omgeoMap)
        for omkey, launches in self.degg_map.items():
            self.assertIn(omkey, readout_map)
            self.assertEqual(len(launches), len(readout_map[omkey]))

        readout_map = UpgradeReadoutSeriesMap(self.mdom_map, self.omgeoMap)
        for omkey, launches in self.mdom_map.items():
            self.assertIn(omkey, readout_map)
            self.assertEqual(len(launches), len(readout_map[omkey]))

        readout_map = UpgradeReadoutSeriesMap(self.xdom_map, self.omgeoMap)
        for omkey, hits in self.xdom_map.items():
            self.assertIn(omkey, readout_map)
            self.assertEqual(len(hits), len(readout_map[omkey]))

    def test_merge(self):
        readout_map = UpgradeReadoutSeriesMap(self.degg_map, self.omgeoMap)
        readout_map.merge(UpgradeReadoutSeriesMap(self.mdom_map, self.omgeoMap))
        readout_map.merge(UpgradeReadoutSeriesMap(self.xdom_map, self.omgeoMap))

        for omkey in (self.degg_omkeys + self.mdom_omkeys + self.xdom_omkeys):
            nhits = (len(self.degg_map.get(omkey, []))
                     + len(self.mdom_map.get(omkey, []))
                     + len(self.xdom_map.get(omkey, [])))
            self.assertIn(omkey, readout_map)
            self.assertEqual(nhits, len(readout_map[omkey]))

    def compare_maps(self, mapA, mapB):
        self.assertEqual(mapA.keys(), mapB.keys())

        # The values() here are the HitSeries objects
        for hitsA, hitsB in zip(mapA.values(), mapB.values()):
            self.assertEqual(len(hitsA), len(hitsB))
            for a, b in zip(hitsA, hitsB):
                self.compare_hits(a, b)
            
    def test_extract(self):
        readout_map = UpgradeReadoutSeriesMap(self.degg_map, self.omgeoMap)
        readout_map.merge(UpgradeReadoutSeriesMap(self.mdom_map, self.omgeoMap))
        readout_map.merge(UpgradeReadoutSeriesMap(self.xdom_map, self.omgeoMap))

        self.compare_maps(self.degg_map, readout_map.to_I3DEggLaunchSeriesMap())
        self.compare_maps(self.mdom_map, readout_map.to_I3mDOMLaunchSeriesMap())
        self.compare_maps(self.xdom_map, readout_map.to_I3XDOMHitSeriesMap())
        
if __name__ == '__main__':
    unittest.main()
