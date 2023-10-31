#!/usr/bin/env python3

# note: this has some awkward asserts. this is intentional why? i
# don't know. possibly a result of the ancient python "test harness"
# that never got developed. they're a direct port from the old way to
# unittest. regardless, they work. they're just ugly. hence:
#
# ruff: noqa: SIM201, SIM202

import unittest
from icecube import dataclasses


class I3StringCompare(unittest.TestCase):
    def testI3StringVsI3String(self):
        self.assertEqual(dataclasses.I3String('test'), dataclasses.I3String('test'), "I3String == I3String failed")
        self.assertTrue(not (dataclasses.I3String('test') == dataclasses.I3String('tests')), "I3String not == I3String failed")

        self.assertNotEqual(dataclasses.I3String('test'), dataclasses.I3String('tests'), "I3String != I3String failed")
        self.assertTrue(not (dataclasses.I3String('test') != dataclasses.I3String('test')), "I3String not != I3String failed")

    def testI3StringVsString(self):
        self.assertEqual(dataclasses.I3String('test'), 'test', "I3String == string failed")
        self.assertTrue(not (dataclasses.I3String('test') == 'tests'), "I3String not == string failed")

        self.assertNotEqual(dataclasses.I3String('test'), 'tests', "I3String != string failed")
        self.assertTrue(not (dataclasses.I3String('test') != 'test'), "I3String not != string failed")


if __name__ == '__main__':
    unittest.main()
