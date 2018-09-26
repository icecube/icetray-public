#!/usr/bin/env python

import unittest
from icecube import dataclasses
from I3Tray import I3Units

class TestI3DetectorStatus(unittest.TestCase):

    def test_I3DetectorStatus_equality(self):
        stat1 = dataclasses.I3DetectorStatus()        
        stat1.dom_status = dataclasses.I3DOMStatusMap()
        stat1.trigger_status = dataclasses.I3TriggerStatusMap()
        stat1.start_time = dataclasses.I3Time()
        stat1.end_time = dataclasses.I3Time()

        stat2 = dataclasses.I3DetectorStatus()        
        stat2.dom_status = dataclasses.I3DOMStatusMap()
        stat2.trigger_status = dataclasses.I3TriggerStatusMap()
        stat2.start_time = dataclasses.I3Time()
        stat2.end_time = dataclasses.I3Time()

        self.assertEqual(stat1, stat2, "these should be the same.")
                         

unittest.main()        
