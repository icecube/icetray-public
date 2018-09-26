#!/usr/bin/env python

import unittest
from icecube import dataclasses

class TestI3Waveform(unittest.TestCase):


    def test_I3Waveform_string(self):
        wave1 = dataclasses.I3Waveform()
        wave2 = dataclasses.I3Waveform()

        wave1.time = 0.123
        wave2.time = 0.123
        wave1.hlc  = True
        wave2.hlc  = True

        self.assertEqual(wave1.__str__(), wave2.__str__(), "these should be the same.")


unittest.main()
