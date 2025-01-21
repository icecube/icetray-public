#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2025 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import unittest

from icecube.dataclasses import I3Position, I3Direction
from icecube.phys_services import ExtrudedPolygon


class ExtrudedPolygonTest(unittest.TestCase):
    def setUp(self):
        corners = [
            (0, 0, 0),
            (0, 0, 1),
            (0, 1, 0),
            (0, 1, 1),
            (1, 0, 0),
            (1, 0, 1),
            (1, 1, 0),
            (1, 1, 1),
        ]
        self.surface = ExtrudedPolygon([I3Position(*corner) - I3Position(0.5, 0.5, 0.5) for corner in corners])

        return super().setUp()

    def testIntersection(self):
        pos = [
            I3Position(*p)
            for p in [
                (1, 0, 0),
                (0, 1, 0),
                (-1, 0, 0),
                (0, -1, 0),
                (0, 0, 1),
                (0, 0, -1),
            ]
        ]
        for p in pos:
            # case 1: pointing towards the surface
            dir = I3Direction(-p)
            intersection = self.surface.intersection(p, dir)
            self.assertEqual(intersection.first, 0.5)
            self.assertEqual(intersection.second, 1.5)

            # case 2: pointing away from the surface
            dir = I3Direction(p)
            intersection = self.surface.intersection(p, dir)
            self.assertEqual(intersection.first, -1.5)
            self.assertEqual(intersection.second, -0.5)


if __name__ == "__main__":
    unittest.main()
