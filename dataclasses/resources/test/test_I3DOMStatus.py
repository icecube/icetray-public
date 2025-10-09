#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import unittest
from icecube import dataclasses
from icecube.icetray import I3Units

class TestI3DOMStatus(unittest.TestCase):

    def test_I3DOMStatus_equality_default_ctor(self):
        ds1 = dataclasses.I3DOMStatus()
        ds2 = dataclasses.I3DOMStatus()
        self.assertEqual(ds1, ds2, "these should be the same.")

unittest.main()
