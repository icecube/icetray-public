#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import unittest
from icecube import dataclasses

class I3AntennaKey(unittest.TestCase):
  def test_Operator(self):
    key1 = dataclasses.AntennaKey(0,0)
    key2 = dataclasses.AntennaKey(0,1)
    key3 = dataclasses.AntennaKey(1,0)
    key4 = dataclasses.AntennaKey(1,1)

    self.assertLess(key1, key2)
    self.assertLess(key1, key3)
    self.assertLess(key1, key4)
    self.assertLess(key2, key3)
    self.assertLess(key2, key4)
    self.assertLess(key3, key4)

    self.assertFalse(key2 < key1)
    self.assertFalse(key3 < key1)
    self.assertFalse(key4 < key1)
    self.assertFalse(key3 < key2)
    self.assertFalse(key4 < key2)
    self.assertFalse(key4 < key3)

  def test_Print(self):
    print(dataclasses.AntennaKey(0,0))

unittest.main()
