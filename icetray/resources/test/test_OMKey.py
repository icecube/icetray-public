#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import unittest
from icecube import icetray

class test_OMKey(unittest.TestCase):

    def test_OMKey(self):
        omkey = icetray.OMKey(21,30,1)
        self.assertEqual( omkey[0], 21, "got the wrong string")
        self.assertEqual( omkey[1], 30, "got the wrong module")
        self.assertEqual( omkey[2], 1, "got the wrong pmt")

unittest.main()




