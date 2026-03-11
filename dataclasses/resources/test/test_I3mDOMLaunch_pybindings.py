#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2025 The IceTray Contributors
# SPDX-License-Identifier: BSD-2-Clause
#
# Date 2025-06-30
# Author stuttard


import unittest
import numpy as np

from icecube import icetray,dataclasses

class I3mDOMLaunchTest(unittest.TestCase):
    def setUp(self):

        self.frame = icetray.I3Frame(icetray.I3Frame.Physics)

        launch = dataclasses.I3mDOMLaunch()
        launch.time = 5.
        launch.baseline_value = 10.
        launch.lc_flags = dataclasses.UpgradeLCFlags.SingleModuleLC
        launch.n_presamples = 20
        launch.adc_data = [1,2,3]

        tot_hit = dataclasses.I3mDOMLaunch.TOTHit()
        tot_hit.time = 1.
        tot_hit.tot = 2.
        launch.tot_hits.append(tot_hit)

        launch_map = dataclasses.I3mDOMLaunchSeriesMap()
        om_key = icetray.OMKey(1, 2, 3)
        launch_map[om_key] = [launch]

        self.frame['launch_map'] = launch_map

    def testLaunch(self):
        
        # Test map
        launch_map = self.frame['launch_map']
        self.assertEqual(len(launch_map), 1)

        # Test OM key storage
        om_key = list(launch_map.keys())[0]
        self.assertEqual(om_key.string, 1)
        self.assertEqual(om_key.om, 2)
        self.assertEqual(om_key.pmt, 3)

        # Test launch series
        launch_series = list(launch_map.values())[0]
        self.assertEqual(len(launch_series), 1)
        launch = launch_series[0]

        # Test launch time
        self.assertEqual(launch.time, 5.)
        self.assertEqual(launch.valid_time, 5.)
        self.assertEqual(launch.has_valid_time, True)
        launch.time = 6.
        self.assertEqual(launch.time, 6.)
        self.assertEqual(launch.has_valid_time, True)
        launch.set_time(7., False)
        self.assertEqual(launch.time, 7.)
        self.assertEqual(launch.has_valid_time, False)

        # Test launch baseline
        self.assertEqual(launch.baseline_value, 10.)
        self.assertEqual(launch.valid_baseline_value, 10.)
        self.assertEqual(launch.has_valid_baseline_value, True)
        launch.baseline_value = 11.
        self.assertEqual(launch.baseline_value, 11.)
        self.assertEqual(launch.has_valid_baseline_value, True)
        launch.set_baseline_value(12., False)
        self.assertEqual(launch.baseline_value, 12.)
        self.assertEqual(launch.has_valid_baseline_value, False)

        # Test LC bit
        self.assertEqual(launch.lc_flags, dataclasses.UpgradeLCFlags.SingleModuleLC)
        launch.lc_flags = dataclasses.UpgradeLCFlags.NoLC
        self.assertEqual(launch.lc_flags, dataclasses.UpgradeLCFlags.NoLC)

        # Test num presamples
        self.assertEqual(launch.n_presamples, 20)
        launch.n_presamples = 21
        self.assertEqual(launch.n_presamples, 21)

        # Test ADC data
        adc_data = launch.adc_data
        self.assertEqual(len(adc_data), 3)
        self.assertEqual(adc_data[0], 1)
        self.assertEqual(adc_data[1], 2)
        self.assertEqual(adc_data[2], 3)

        launch.adc_data = [10,11]
        self.assertEqual(len(adc_data), 2)
        self.assertEqual(adc_data[0], 10)
        self.assertEqual(adc_data[1], 11)

        # Test trigger type
        self.assertEqual(launch.trigger_type, dataclasses.I3mDOMLaunch.TriggerType.UNDEFINED)
        launch.trigger_type = dataclasses.I3mDOMLaunch.TriggerType.SOFTWARE
        self.assertEqual(launch.trigger_type, dataclasses.I3mDOMLaunch.TriggerType.SOFTWARE)
        launch.trigger_type = dataclasses.I3mDOMLaunch.TriggerType.ADC_THRESHOLD
        self.assertEqual(launch.trigger_type, dataclasses.I3mDOMLaunch.TriggerType.ADC_THRESHOLD)
        launch.trigger_type = dataclasses.I3mDOMLaunch.TriggerType.DISCRIMINATOR
        self.assertEqual(launch.trigger_type, dataclasses.I3mDOMLaunch.TriggerType.DISCRIMINATOR)
        launch.trigger_type = dataclasses.I3mDOMLaunch.TriggerType.EXTERNAL
        self.assertEqual(launch.trigger_type, dataclasses.I3mDOMLaunch.TriggerType.EXTERNAL)

        # Test TOTHits
        tot_hits = launch.tot_hits
        self.assertEqual(len(tot_hits), 1)
        self.assertEqual(tot_hits[0].time, 1.)
        self.assertEqual(tot_hits[0].tot, 2.)
        tot_hits[0].time = 3.
        tot_hits[0].tot = 4.
        self.assertEqual(tot_hits[0].time, 3.)
        self.assertEqual(tot_hits[0].tot, 4.)


if __name__ == '__main__':
    unittest.main()
