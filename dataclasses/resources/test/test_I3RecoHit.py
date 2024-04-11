#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import unittest
from icecube import dataclasses
from icecube.icetray import I3Units

class TestI3RecoHit(unittest.TestCase):

    def test_equality(self):
        hit1 = dataclasses.I3RecoHit()
        hit1.time = 3.3
        hit1.id = 1
        hit1.source_index = 1

        hit2 = dataclasses.I3RecoHit()
        hit2.time = 3.3
        hit2.id = 1
        hit2.source_index = 1

        self.assertEqual(hit1, hit2, "these should be the same.")


unittest.main()
