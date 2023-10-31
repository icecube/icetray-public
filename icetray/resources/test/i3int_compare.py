#!/usr/bin/env python3

import unittest
from icecube import icetray


class I3IntCompare(unittest.TestCase):
    def testI3IntVsI3Int(self):
        self.assertEqual(icetray.I3Int(12), icetray.I3Int(12), "I3Int == I3Int failed")
        self.assertNotEqual(icetray.I3Int(12), icetray.I3Int(13), "I3Int != I3Int failed")
        self.assertLess(icetray.I3Int(12), icetray.I3Int(13), "I3Int <  I3Int failed")
        self.assertLessEqual(icetray.I3Int(12), icetray.I3Int(13), "I3Int <= I3Int failed")
        self.assertGreater(icetray.I3Int(14), icetray.I3Int(13), "I3Int >  I3Int failed")
        self.assertGreaterEqual(icetray.I3Int(14), icetray.I3Int(13), "I3Int >= I3Int failed")

    def testI3IntVsInt(self):
        self.assertEqual(icetray.I3Int(12), 12, "I3Int == int failed")
        self.assertNotEqual(icetray.I3Int(12), 13, "I3Int != int failed")
        self.assertLess(icetray.I3Int(12), 13, "I3Int <  int failed")
        self.assertLessEqual(icetray.I3Int(12), 13, "I3Int <= int failed")
        self.assertGreater(icetray.I3Int(14), 13, "I3Int >  int failed")
        self.assertGreaterEqual(icetray.I3Int(14), 13, "I3Int >= int failed")

    def testI3IntVsFloat(self):
        self.assertEqual(icetray.I3Int(12), 12.0, "I3Int == float failed")
        self.assertNotEqual(icetray.I3Int(12), 13.0, "I3Int != float failed")
        self.assertLess(icetray.I3Int(12), 13.0, "I3Int <  float failed")
        self.assertLessEqual(icetray.I3Int(12), 13.0, "I3Int <= float failed")
        self.assertGreater(icetray.I3Int(14), 13.0, "I3Int >  float failed")
        self.assertGreaterEqual(icetray.I3Int(14), 13.0, "I3Int >= float failed")

    def testI3IntTruthiness(self):
        # falsehood of zero
        self.assertFalse(icetray.I3Int(0), "I3Int == False failed")
        self.assertEqual(icetray.I3Int(0), False, "I3Int == False failed")
        self.assertNotEqual(icetray.I3Int(0), True, "I3Int != True failed")

        # truth of not zero
        self.assertTrue(icetray.I3Int(1), "I3Int == False failed")
        self.assertEqual(icetray.I3Int(1), True, "I3Int == True failed")
        self.assertNotEqual(icetray.I3Int(1), False, "I3Int != False failed")

        # test bool cast equivalency
        self.assertFalse(bool(icetray.I3Int(0)), "I3Int == False failed")
        self.assertTrue(bool(icetray.I3Int(1)), "I3Int == True failed")

        # do we get a True/False object from the cast
        self.assertIs(bool(icetray.I3Int(0)), False, "I3Int == False failed")
        self.assertIs(bool(icetray.I3Int(1)), True, "I3Int == True failed")


if __name__ == "__main__":
    unittest.main()
