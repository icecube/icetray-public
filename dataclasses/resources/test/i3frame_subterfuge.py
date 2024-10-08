#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from icecube import icetray, dataclasses
import unittest

class I3FrameSubterfuge(unittest.TestCase):
    def setUp(self):
        rpsm = dataclasses.I3RecoPulseSeriesMap()
        p = dataclasses.I3RecoPulse()
        p.time = 0
        p.charge = 1
        p.width = 1
        p.flags = p.PulseFlags.LC | p.PulseFlags.ATWD
        rpsm[icetray.OMKey(1,1)] = dataclasses.I3RecoPulseSeries([p])
        sdst = dataclasses.I3SuperDST(rpsm)
        frame = icetray.I3Frame()
        frame['sdst'] = sdst
        mask = dataclasses.I3RecoPulseSeriesMapMask(frame, 'sdst')
        frame['mask'] = mask
        self.frame = frame
    def testSuperDST(self):
        """SuperDST can be automatically rehydrated."""
        rpsm = dataclasses.I3RecoPulseSeriesMap.from_frame(self.frame, 'sdst')
        self.assertIsInstance(rpsm, dataclasses.I3RecoPulseSeriesMap)
        self.assertEqual(len(rpsm), 1)
        self.assertEqual(list(rpsm.keys())[0], icetray.OMKey(1,1))

    def testMask(self):
        """Masks can be automatically applied."""
        rpsm = dataclasses.I3RecoPulseSeriesMap.from_frame(self.frame, 'mask')
        self.assertIsInstance(rpsm, dataclasses.I3RecoPulseSeriesMap)
        self.assertEqual(len(rpsm), 1)
        self.assertEqual(list(rpsm.keys())[0], icetray.OMKey(1,1))


if __name__ == "__main__":
    unittest.main()
