#!/usr/bin/env python

import unittest
from icecube import dataclasses
from I3Tray import I3Units

class TestI3Calibration(unittest.TestCase):

    def test_I3Calibration_equality(self):
        cal1 = dataclasses.I3Calibration()        
        cal1.dom_cal = dataclasses.I3DOMCalibrationMap()
        cal1.vem_cal = dataclasses.I3VEMCalibrationMap()
        cal1.start_time = dataclasses.I3Time()
        cal1.end_time = dataclasses.I3Time()

        cal2 = dataclasses.I3Calibration()        
        cal2.dom_cal = dataclasses.I3DOMCalibrationMap()
        cal2.vem_cal = dataclasses.I3VEMCalibrationMap()
        cal2.start_time = dataclasses.I3Time()
        cal2.end_time = dataclasses.I3Time()

        self.assertEqual(cal1, cal2, "these should be the same.")
                         

unittest.main()        
