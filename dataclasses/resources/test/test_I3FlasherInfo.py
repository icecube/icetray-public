#!/usr/bin/env python

import unittest
from icecube import dataclasses
from icecube import icetray
from icecube.icetray import OMKey

class TestI3FlasherInfo(unittest.TestCase):


    def test_I3FlasherInfo_equality(self):

        fi1 = dataclasses.I3FlasherInfo()
        fi2 = dataclasses.I3FlasherInfo()

        fi1.flashing_om    = OMKey(2,1,0)
        fi2.flashing_om    = OMKey(2,1,0)
        fi1.flash_time     = 1.0
        fi2.flash_time     = 1.0
        fi1.atwd_bin_size  = 1.0
        fi2.atwd_bin_size  = 1.0
        fi1.led_brightness = 1
        fi2.led_brightness = 1
        fi1.mask           = 1
        fi2.mask           = 1
        fi1.width          = 1
        fi2.width          = 1
        fi1.rate           = 1
        fi2.rate           = 1
        fi1.raw_atwd3      = [ 1, 2, 3 ]
        fi2.raw_atwd3      = [ 1, 2, 3 ]

        self.assertTrue(fi1==fi2, "this should be true.")


    def test_I3FlasherInfo_inequality(self):

        fi1 = dataclasses.I3FlasherInfo()
        fi2 = dataclasses.I3FlasherInfo()

        fi1.flashing_om    = OMKey(3,1,0)
        fi2.flashing_om    = OMKey(2,1,0)
        fi1.flash_time     = 1.0
        fi2.flash_time     = 1.0
        fi1.atwd_bin_size  = 1.0
        fi2.atwd_bin_size  = 1.0
        fi1.led_brightness = 1
        fi2.led_brightness = 1
        fi1.mask           = 1
        fi2.mask           = 1
        fi1.width          = 1
        fi2.width          = 1
        fi1.rate           = 1
        fi2.rate           = 1
        fi1.raw_atwd3      = [ 1, 2, 3 ]
        fi2.raw_atwd3      = [ 1, 2, 3 ]

        self.assertFalse(fi1==fi2, "this should be false.")


unittest.main()

