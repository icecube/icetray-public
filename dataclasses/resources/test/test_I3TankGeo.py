#!/usr/bin/env python

import unittest
from icecube import dataclasses
from I3Tray import I3Units

class TestI3TankGeo(unittest.TestCase):

    def test_I3TankGeo_equality(self):
        tankgeo1 = dataclasses.I3TankGeo()
        tankgeo1.tanktype = dataclasses.I3TankGeo.TankType.Tyvek_Lined
        tankgeo1.position = dataclasses.I3Position(0,0,0)
        tankgeo1.orientation = 1.0
        tankgeo1.snowheight = 42.
        tankgeo1.tankradius = 3.14
        tankgeo1.tankheight = 1.618
        tankgeo1.fillheight = 1.20205

        tankgeo2 = dataclasses.I3TankGeo()
        tankgeo2.tanktype = dataclasses.I3TankGeo.TankType.Tyvek_Lined
        tankgeo2.position = dataclasses.I3Position(0,0,0)
        tankgeo2.orientation = 1.0
        tankgeo2.snowheight = 42.
        tankgeo2.tankradius = 3.14
        tankgeo2.tankheight = 1.618
        tankgeo2.fillheight = 1.20205

        self.assertEqual(tankgeo1, tankgeo2, "these should be the same.")

    def test_I3TankGeo_equality_default_constructed(self):
        tankgeo1 = dataclasses.I3TankGeo()
        tankgeo2 = dataclasses.I3TankGeo()
        self.assertEqual(tankgeo1, tankgeo2, "these should be the same.")


unittest.main()        
