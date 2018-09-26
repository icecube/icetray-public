#!/usr/bin/env python

import unittest
from icecube import dataclasses
from I3Tray import I3Units

class TestI3ModuleGeo(unittest.TestCase):

    def test_I3ModuleGeo(self):
        module = dataclasses.I3ModuleGeo()
        module.pos = dataclasses.I3Position(0,0,0)
        module.orientation = dataclasses.I3Orientation(1,0,0,0,1,0)
        module.radius = 99.
        module.module_type = dataclasses.I3ModuleGeo.IceCube
        self.assertEqual(module.dir,
                         dataclasses.I3Direction(90 * I3Units.degree, 180 * I3Units.degree),
                         "didn't get the direction i was looking for.")
        self.assertEqual(module.radius, 99.,
                         "set/get isn't working.")
        self.assertEqual(module.pos, dataclasses.I3Position(0,0,0),
                         "set/get isn't working.")
        self.assertEqual(module.orientation, dataclasses.I3Orientation(1,0,0,0,1,0),
                         "set/get isn't working.")
        self.assertEqual(module.module_type, dataclasses.I3ModuleGeo.IceCube,
                         "set/get isn't working.")


        module2 = dataclasses.I3ModuleGeo()
        module2.pos = dataclasses.I3Position(0,0,0)
        module2.orientation = dataclasses.I3Orientation(1,0,0,0,1,0)
        module2.radius = 99.
        module2.module_type = dataclasses.I3ModuleGeo.IceCube
        self.assertEqual(module,module2,
                         "testing the equal operator.")



unittest.main()        
