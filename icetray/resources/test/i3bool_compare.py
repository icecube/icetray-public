#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import unittest
from icecube import icetray


class I3BoolCompare(unittest.TestCase):
    def testI3Bool(self):
        # we're testing for _equality_, not _identity_
        self.assertEqual(icetray.I3Bool(True), True, "I3Bool == True failed")
        self.assertEqual(icetray.I3Bool(False), False, "I3Bool == False failed")

        self.assertNotEqual(icetray.I3Bool(True), False, "I3Bool != False failed")
        self.assertNotEqual(icetray.I3Bool(False), True, "I3Bool != True failed")

        self.assertTrue(bool(icetray.I3Bool(True)), "I3Bool == True failed")
        self.assertFalse(bool(icetray.I3Bool(False)), "I3Bool == False failed")

        # test for _identity_
        self.assertIs(bool(icetray.I3Bool(True)), True, "I3Bool == True failed")
        self.assertIs(bool(icetray.I3Bool(False)), False, "I3Bool == False failed")

        # test repr
        b = repr(icetray.I3Bool(True))
        self.assertTrue('true' in b.lower(), "I3Bool does not repr true")
        b = repr(icetray.I3Bool(False))
        self.assertTrue('false' in b.lower(), "I3Bool does not repr false")

        # test str
        b = icetray.I3Bool(True).__str__()
        self.assertTrue('true' in b.lower(), "I3Bool does not print true")
        b = icetray.I3Bool(False).__str__()
        self.assertTrue('false' in b.lower(), "I3Bool does not print false")


if __name__ == "__main__":
    unittest.main()
