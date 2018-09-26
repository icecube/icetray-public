#!/usr/bin/env python

import unittest
from icecube import dataclasses

class TestI3FilterResult(unittest.TestCase):


    def test_I3FilterResult_string(self):
        fr1 = dataclasses.I3FilterResult()
        fr2 = dataclasses.I3FilterResult()

        fr1.condition_passed = 1
        fr2.condition_passed = 1
        fr1.prescale_passed  = 1
        fr2.prescale_passed  = 1

        self.assertEqual(fr1.__str__(), fr2.__str__(), "these should be the same.")


    def test_I3FilterResult_bool(self):
        fr1 = dataclasses.I3FilterResult()

        fr1.condition_passed = 1
        fr1.prescale_passed  = 1

        self.assertTrue(fr1.__nonzero__(), "this should be true.")


unittest.main()
