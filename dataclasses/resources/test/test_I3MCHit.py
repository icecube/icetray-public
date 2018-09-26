#!/usr/bin/env python

import unittest
from icecube import dataclasses

class TestI3MCHit(unittest.TestCase):


    def test_I3MCHit_string(self):
        hit1 = dataclasses.I3MCHit()
        hit2 = dataclasses.I3MCHit()

        hit1.time               = 0.123
        hit2.time               = 0.123
        hit1.hit_id             = 0
        hit2.hit_id             = 0
        hit1.npe                = 10
        hit2.npe                = 10
        hit1.charge             = 16.67
        hit2.charge             = 16.67
        hit1.cherenkov_distance = 10.
        hit2.cherenkov_distance = 10.

        self.assertEqual(hit1.__str__(), hit2.__str__(), "these should be the same.")


unittest.main()
