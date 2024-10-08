#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import unittest
from icecube import dataclasses
from icecube.icetray import I3Units

class TestI3Waveform(unittest.TestCase):

    def test_I3Waveform_Equality_Operator(self):
        wave1 = dataclasses.I3Waveform()
        wave1.time = 123
        wave1.bin_width = 100

        wave2 = dataclasses.I3Waveform()
        wave2.time = 234
        wave2.bin_width = 200

        wave3 = dataclasses.I3Waveform()
        wave3.time = 123
        wave3.bin_width = 100

        self.assertEqual(wave1, wave1, 'these should be equal')
        self.assertNotEqual(wave1, wave2, 'these should not be equal!')
        self.assertEqual(wave1, wave3, 'these should be equal!')

unittest.main()
