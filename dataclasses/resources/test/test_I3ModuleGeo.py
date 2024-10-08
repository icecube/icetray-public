#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import unittest
import numpy as np
from icecube import dataclasses
from icecube.icetray import I3Units

class TestI3ModuleGeo(unittest.TestCase):

    def test_I3ModuleGeo(self):
        module = dataclasses.I3ModuleGeo()
        module.pos = dataclasses.I3Position(0,0,0)
        module.orientation = dataclasses.I3Orientation(1,0,0,0,1,0)
        module.module_type = dataclasses.I3ModuleGeo.IceCube
        self.assertEqual(module.dir,
                         dataclasses.I3Direction(90 * I3Units.degree, 180 * I3Units.degree),
                         "didn't get the direction i was looking for.")
        self.assertEqual(module.pos, dataclasses.I3Position(0,0,0),
                         "set/get isn't working.")
        self.assertEqual(module.orientation, dataclasses.I3Orientation(1,0,0,0,1,0),
                         "set/get isn't working.")
        self.assertEqual(module.module_type, dataclasses.I3ModuleGeo.IceCube,
                         "set/get isn't working.")
        self.assertEqual(module.radius, 0.1651,
                         "set/get isn't working.")


        module2 = dataclasses.I3ModuleGeo()
        module2.pos = dataclasses.I3Position(0,0,0)
        module2.orientation = dataclasses.I3Orientation(1,0,0,0,1,0)
        module2.module_type = dataclasses.I3ModuleGeo.IceCube
        self.assertEqual(module,module2,
                         "testing the equal operator.")

        module.radius = 99.
        self.assertEqual(module.radius, 99.,
                         "set/get isn't working.")
        self.assertEqual(module.rr, 99.,
                         "set/get isn't working.")
        self.assertEqual(module.rz, 99.,
                         "set/get isn't working.")
        self.assertTrue(np.isnan(module.height),
                        "set/get isn't working.")
        module.rr = 9.
        self.assertEqual(module.rr, 9.,
                         "set/get isn't working.")
        self.assertEqual(module.rz, 99.,
                         "set/get isn't working.")
        self.assertTrue(np.isnan(module.height),
                        "set/get isn't working.")
        module.rz = 9.
        self.assertEqual(module.rr, 9.,
                         "set/get isn't working.")
        self.assertEqual(module.rz, 9.,
                         "set/get isn't working.")
        self.assertTrue(np.isnan(module.height),
                        "set/get isn't working.")
        module.height = 9.
        self.assertEqual(module.rr, 9.,
                         "set/get isn't working.")
        self.assertTrue(np.isnan(module.rz),
                        "set/get isn't working.")
        self.assertEqual(module.height, 9.,
                         "set/get isn't working.")
        module.rz = 9.
        self.assertTrue(np.isnan(module.height),
                        "set/get isn't working.")
        self.assertEqual(module.rz, 9.,
                         "set/get isn't working.")



unittest.main()
