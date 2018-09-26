#!/usr/bin/env python

import unittest
from icecube import dataclasses
from I3Tray import I3Units

class TestI3Geometry(unittest.TestCase):

    def test_I3Geometry_equality(self):
        geo1 = dataclasses.I3Geometry()        
        geo1.omgeo = dataclasses.I3OMGeoMap()
        geo1.stationgeo = dataclasses.I3StationGeoMap()
        geo1.start_time = dataclasses.I3Time()
        geo1.end_time = dataclasses.I3Time()

        geo2 = dataclasses.I3Geometry()        
        geo2.omgeo = dataclasses.I3OMGeoMap()
        geo2.stationgeo = dataclasses.I3StationGeoMap()
        geo2.start_time = dataclasses.I3Time()
        geo2.end_time = dataclasses.I3Time()

        self.assertEqual(geo1, geo2, "these should be the same.")
                         

unittest.main()        
