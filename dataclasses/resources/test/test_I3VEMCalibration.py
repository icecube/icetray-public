#!/usr/bin/env python

import unittest
from icecube import dataclasses
from I3Tray import I3Units

class TestI3VEMCalibration(unittest.TestCase):

    def test_I3VEMCalibration_equality(self):
        cal1 = dataclasses.I3VEMCalibration()        
        cal1.pe_per_vem = 3.14
        cal1.mu_peak_width = 1.618
        cal1.hglg_cross_over = 1.20205 
        cal1.corr_factor = 42.

        cal2 = dataclasses.I3VEMCalibration()        
        cal2.pe_per_vem = 3.14
        cal2.mu_peak_width = 1.618
        cal2.hglg_cross_over = 1.20205 
        cal2.corr_factor = 42.

        self.assertEqual(cal1, cal2, "these should be the same.")
                         
unittest.main()        
