#!/usr/bin/env python

import unittest
from icecube import dataclasses
from I3Tray import I3Units

class TestI3TriggerStatus(unittest.TestCase):

    def test_I3TriggerReadoutConfig_equality(self):
        stat1 = dataclasses.I3TriggerStatus.I3TriggerReadoutConfig()
        stat1.readout_time_minus = 3.14
        stat1.readout_time_plus = 1.618
        stat1.readout_time_offset = 1.20205

        stat2 = dataclasses.I3TriggerStatus.I3TriggerReadoutConfig()
        stat2.readout_time_minus = 3.14
        stat2.readout_time_plus = 1.618
        stat2.readout_time_offset = 1.20205

        self.assertEqual(stat1, stat2, "these should be the same.")
                         
unittest.main()        
