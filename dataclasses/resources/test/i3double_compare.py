#!/usr/bin/env python3

import unittest
from icecube.dataclasses import I3Double


class I3DoubleCompare(unittest.TestCase):
    def testI3DoubleVsI3Double(self):
        self.assertEqual(I3Double(12.0), I3Double(12.0), "I3Double == I3Double failed")
        self.assertNotEqual(I3Double(12.0), I3Double(13), "I3Double != I3Double failed")
        self.assertLess(I3Double(12.0), I3Double(13), "I3Double < I3Double failed")
        self.assertLessEqual(I3Double(12.0), I3Double(13), "I3Double <= I3Double failed")
        self.assertGreater(I3Double(14.0), I3Double(13), "I3Double > I3Double failed")
        self.assertGreaterEqual(I3Double(14.0), I3Double(13), "I3Double >= I3Double failed")

    def testI3DoubleVsInt(self):
        self.assertEqual(I3Double(12.0), 12, "I3Double == int failed")
        self.assertNotEqual(I3Double(12.0), 13, "I3Double != int failed")
        self.assertLess(I3Double(12.0), 13, "I3Double < int failed")
        self.assertLessEqual(I3Double(12.0), 13, "I3Double <= int failed")
        self.assertGreater(I3Double(14.0), 13, "I3Double > int failed")
        self.assertGreaterEqual(I3Double(14.0), 13, "I3Double >= int failed")

    def testI3DoubleVsFloat(self):
        self.assertEqual(I3Double(12.0), 12.0, "I3Double == float failed")
        self.assertNotEqual(I3Double(12.0), 13.0, "I3Double != float failed")
        self.assertLess(I3Double(12.0), 13.0, "I3Double < float failed")
        self.assertLessEqual(I3Double(12.0), 13.0, "I3Double <= float failed")
        self.assertGreater(I3Double(14.0), 13.0, "I3Double > float failed")
        self.assertGreaterEqual(I3Double(14.0), 13.0, "I3Double >= float failed")

    def testI3DoubleTruthiness(self):
        # these should probably be assertTrue/assertFalse
        self.assertEqual(I3Double(0.0), False, "I3Double == False failed")
        self.assertNotEqual(I3Double(0.0), True, "I3Double != True failed")
        self.assertEqual(I3Double(1.0), True, "I3Double == True failed")
        self.assertNotEqual(I3Double(1.0), False, "I3Double != False failed")

        # these test if the result _is_ the boolean object True/False, not equivalency
        self.assertIs(bool(I3Double(0.0)), False, "bool(0) should be false")
        self.assertIs(bool(I3Double(1.0)), True, "bool(1) should be true")


if __name__ == "__main__":
    unittest.main()
