#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import unittest
from icecube.dataclasses import I3UInt64

class TestI3UInt64(unittest.TestCase):
    def test_bool(self):
        self.assertFalse(bool(I3UInt64(0)), "this should be false.")
        self.assertTrue(bool(I3UInt64(1)), "this should be false.")

if __name__ == "__main__":
    unittest.main()
