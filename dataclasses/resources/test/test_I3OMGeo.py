#!/usr/bin/env python

import unittest
from icecube import dataclasses
from I3Tray import I3Units

class TestI3ModuleGeo(unittest.TestCase):

    def test_I3OMGeo(self):
        omgeo1 = dataclasses.I3OMGeo()
        omgeo1.omtype = dataclasses.I3OMGeo.OMType.IceCube
        omgeo1.position = dataclasses.I3Position(0,0,0)
        omgeo1.orientation = dataclasses.I3Orientation(1,0,0,0,1,0)
        omgeo1.area = 42.

        omgeo2 = dataclasses.I3OMGeo()
        omgeo2.omtype = dataclasses.I3OMGeo.OMType.IceCube
        omgeo2.position = dataclasses.I3Position(0,0,0)
        omgeo2.orientation = dataclasses.I3Orientation(1,0,0,0,1,0)
        omgeo2.area = 42.

        self.assertEqual(omgeo1, omgeo2, "these should be the same.")
                         

unittest.main()        
