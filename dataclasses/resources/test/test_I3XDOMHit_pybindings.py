#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2025 The IceTray Contributors
# SPDX-License-Identifier: BSD-2-Clause
#
# Date 2025-06-30
# Author stuttard

import unittest
import numpy as np

from icecube import icetray,dataclasses

class I3XDOMHitTest(unittest.TestCase):
    def setUp(self):

        self.frame = icetray.I3Frame(icetray.I3Frame.Physics)

        hit = dataclasses.I3XDOMHit()
        hit.time = 5.
        hit.charge = 10.
        hit.lc_flags = dataclasses.UpgradeLCFlags.SingleModuleLC

        hit_map = dataclasses.I3XDOMHitSeriesMap()
        om_key = icetray.OMKey(1, 2, 3)
        hit_map[om_key] = [hit]

        self.frame['hit_map'] = hit_map


    def testLaunch(self):
        
        # Test map
        hit_map = self.frame['hit_map']
        self.assertEqual(len(hit_map), 1)

        # Test OM key storage
        om_key = list(hit_map.keys())[0]
        self.assertEqual(om_key.string, 1)
        self.assertEqual(om_key.om, 2)
        self.assertEqual(om_key.pmt, 3)

        # Test hit series
        hit_series = list(hit_map.values())[0]
        self.assertEqual(len(hit_series), 1)
        hit = hit_series[0]

        # Test hit time
        self.assertEqual(hit.time, 5.)
        hit.time = 6.
        self.assertEqual(hit.time, 6.)

        # Test hit charge
        self.assertEqual(hit.charge, 10.)
        hit.charge = 11.
        self.assertEqual(hit.charge, 11.)

        # Test hit LC flag
        self.assertEqual(hit.lc_flags, dataclasses.UpgradeLCFlags.SingleModuleLC)
        hit.lc_flags = dataclasses.UpgradeLCFlags.NoLC
        self.assertEqual(hit.lc_flags, dataclasses.UpgradeLCFlags.NoLC)


if __name__ == '__main__':
    unittest.main()
