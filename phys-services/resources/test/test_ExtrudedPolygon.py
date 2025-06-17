#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2025 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import unittest

from icecube.dataclasses import I3Position, I3Direction, I3Particle
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
    
    def testClosestApproach(self):
        # check that for intersecting trajectories the function returns nan ofr the closest approach

        p = I3Particle()
        p.pos = I3Position(0,0,1)
        p.dir = I3Direction(0,0,-1)
        p.length = 2

        closest_approach = self.surface.closest_approach(p)
        self.assertTrue(closest_approach.distance != closest_approach.distance)
        self.assertTrue(closest_approach.position == I3Position())

        # check for a few non-intersecting particles whether the function returns the correct values

        positions = [(0,0,10), (0,0,-10), (0,10,0), (10,0,0), (10,0,0), (10,10,0), (10,0,3), (10,10,3)]
        directions = [(0,0,-1), (0,0,1), (0,-1,0), (-1,0,0), (-1,1,0), (0,-1,0), (-1,1,0),(0,-1,0)]
        lengths = [3, 2, 9, 4, 15, 5, 15, 5]
        true_closest_approaches = [6.5, 7.5, 0.5, 5.5, 6.36396103067, 10.51189802081, 6.83739716559, 10.80509139249]
        true_closest_approach_positions = [(0,0,7), (0,0,-8), (0,1,0), (6,0,0), (5,5,0), (10,5,0), (5,5,3), (10,5,3)]

        for pos, dir, l, true_clos_dist, true_clos_pos in zip(positions, directions, lengths, true_closest_approaches, true_closest_approach_positions):
            p = I3Particle()
            p.pos = I3Position(*pos)
            p.dir = I3Direction(*dir)
            p.length = l

            closest_approach = self.surface.closest_approach(p)
            self.assertAlmostEqual(closest_approach.distance, true_clos_dist)
            self.assertAlmostEqual(closest_approach.position, I3Position(*true_clos_pos))
            


if __name__ == "__main__":
    unittest.main()
