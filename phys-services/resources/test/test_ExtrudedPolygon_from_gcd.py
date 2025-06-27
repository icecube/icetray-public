#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2025 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import os
import unittest

from icecube.phys_services import ExtrudedPolygon

TEST_DATA = os.path.expandvars("$I3_TESTDATA")


class ExtrudedPolygonTest(unittest.TestCase):
    def setUp(self):
        # instantiate ExtrudedPolygon with a GCD file
        gcd_path = os.path.join(TEST_DATA, "GCD/GeoCalibDetectorStatus_IC86.55697_corrected_V2.i3.gz")
        self.surface = ExtrudedPolygon(gcd_file=gcd_path, padding=50)

        return super().setUp()

    def testGeometry(self):
        # extract corner positions from ExtrudedPolgon
        X = self.surface.x
        Y = self.surface.y
        Z = self.surface.z

        # true corner positions
        X_true = [
            -627.5127380889292, -531.888669833293,
            -373.60043305720245, -276.72205854484184,
            -124.93977005932484, 397.31174264456666,
            452.5181438713232, 591.5942012021857,
            631.5120930456251, 364.92843555787584,
            21.818065330297244, -108.79875789365002,
            -384.1126875368523
        ]
        Y_true = [
            -132.90658615574756, -260.88461591176,
            -456.0361378460895, -574.985009705583,
            -550.8283145218631, -467.67840147445037,
            -324.2747356796085, 40.186175201270686,
            182.34582799911192, 510.40743489507594,
            560.0631496210277, 539.6174872864432,
            496.4497856342782
        ]
        Z_true = [
            -562.8200073242188, 574.5599975585938
        ]

        # check first, that one gets the correct number of coordinates
        self.assertEqual(len(X), len(X_true))
        self.assertEqual(len(Y), len(Y_true))
        self.assertEqual(len(Z), len(Z_true))

        # check second, that one gets the right corrdinate values
        for x, x_true in zip(X, X_true):
            self.assertAlmostEqual(x, x_true)
        for y, y_true in zip(Y, Y_true):
            self.assertAlmostEqual(y, y_true)
        for z, z_true in zip(Z, Z_true):
            self.assertAlmostEqual(z, z_true)


if __name__ == "__main__":
    unittest.main()
