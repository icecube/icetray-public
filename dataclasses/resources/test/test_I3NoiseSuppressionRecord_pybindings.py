#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2025 The IceTray Contributors
# SPDX-License-Identifier: BSD-2-Clause
#
# Date 2025-06-30
# Author stuttard

import unittest
import numpy as np

from icecube import icetray,dataclasses

class I3NoiseSuppressionRecordTest(unittest.TestCase):
    def setUp(self):

        self.frame = icetray.I3Frame(icetray.I3Frame.Physics)

        record = dataclasses.I3NoiseSuppressionRecord()
        record.time = 5.
        record.n_noise_hits = 1
        record.exclusion_window_size = 10
        record.lc_window_size = 11

        record_map = dataclasses.I3NoiseSuppressionRecordSeriesMap()
        om_key = icetray.OMKey(1, 2, 3)
        record_map[om_key] = [record]

        self.frame['record_map'] = record_map


    def testLaunch(self):
        
        # Test map
        record_map = self.frame['record_map']
        self.assertEqual(len(record_map), 1)

        # Test OM key storage
        om_key = list(record_map.keys())[0]
        self.assertEqual(om_key.string, 1)
        self.assertEqual(om_key.om, 2)
        self.assertEqual(om_key.pmt, 3)

        # Test record series
        record_series = list(record_map.values())[0]
        self.assertEqual(len(record_series), 1)
        record = record_series[0]

        # Test record time
        self.assertEqual(record.time, 5.)
        record.time = 6.
        self.assertEqual(record.time, 6.)

        # Test record N
        self.assertEqual(record.n_noise_hits, 1)
        record.n_noise_hits = 2
        self.assertEqual(record.n_noise_hits, 2)
        
        # Test record size
        self.assertEqual(record.exclusion_window_size, 10)
        record.exclusion_window_size = 20
        self.assertEqual(record.exclusion_window_size, 20)

        # Test record LC size
        self.assertEqual(record.lc_window_size, 11)
        record.lc_window_size = 22
        self.assertEqual(record.lc_window_size, 22)


if __name__ == '__main__':
    unittest.main()
