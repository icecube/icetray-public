#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import os.path
import unittest

from icecube.dataclasses import I3Double
from icecube.shovelart import RangeSetting, ChoiceSetting, KeySetting, FileSetting

class TestSettingTypes( unittest.TestCase ):

    def test_RangeSetting(self):
        s = RangeSetting(-1.5, 1.5, 20, -0.75)
        self.assertEqual(s.value, -0.75)
        self.assertEqual(s.value, eval(repr(s)))

    def test_ChoiceSetting(self):
        slst = (ChoiceSetting(("a", "b", "c"), 1),
             ChoiceSetting("a;b;c", 1))
        for s in slst:
            self.assertEqual(s.value, 1)
            self.assertEqual(s.value, eval(repr(s)))
            self.assertEqual(s.options, ("a", "b", "c"))

    def test_KeySetting(self):
        slst = ( KeySetting(I3Double, "some_val"),
              KeySetting("I3Int", "some_val"),
              KeySetting((I3Double, "I3Int"), "some_val") )
        for s in slst:
            self.assertEqual(s.value, "some_val")
            self.assertEqual(s.value, eval(repr(s)))

    def test_FileSetting(self):
        s = FileSetting("some_val")
        self.assertEqual(s.value, "some_val")
        self.assertEqual(s.value, eval(repr(s)))

if __name__ == "__main__":
    unittest.main()
