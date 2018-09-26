#!/usr/bin/env python

import unittest
import ctypes

from icecube import dataclasses

class TestI3EventHeader(unittest.TestCase):
    def test_01(self):
        uint_max = 2**(ctypes.sizeof(ctypes.c_uint(0))*8)-1
        for _ in range(1000):
            h = dataclasses.I3EventHeader()
            self.assertEqual(h.run_id, uint_max)
            self.assertEqual(h.sub_run_id, uint_max)
            self.assertEqual(h.event_id, uint_max)
            self.assertEqual(h.sub_event_id, 0)
            self.assertEqual(h.sub_event_stream, "")
            self.assertEqual(h.state, h.StateType.OK)


if __name__ == '__main__':
    unittest.main()
