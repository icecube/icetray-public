#!/usr/bin/env python

import unittest
import sys
import pickle
import os
import tempfile

from icecube import icetray,dataclasses,dataio

class I3RecoPulseSeriesMapMaskTest(unittest.TestCase):
    def setUp(self):
        super(I3RecoPulseSeriesMapMaskTest, self).setUp()

        # create a pulse series map with a known bad
        # 0 width case
        self.pulses = dataclasses.I3RecoPulseSeriesMap()
        key1 = icetray.OMKey(42, 7)
        vec = dataclasses.I3RecoPulseSeries()
        pulse = dataclasses.I3RecoPulse()
        pulse.time = 10218.3
        pulse.charge = 11.3792
        pulse.width = 0.833585
        vec.append(pulse)
        pulse.time = 10226.6
        pulse.charge = 28.1466
        pulse.width = 0.833585
        vec.append(pulse)
        pulse.time = 10227.4
        pulse.charge = 18.5683
        pulse.width = 0.833585
        vec.append(pulse)
        self.pulses[key1] = vec

    def test_0_width(self):
        #print('before')
        #print(self.pulses)
        sdst = dataclasses.I3SuperDST(self.pulses)
        unpack = sdst.unpack()
        #print('----')
        #print('after')
        #print(unpack)
        if any(p.width == 0 for pulses in unpack.values() for p in pulses):
            raise Exception('0 width')
            

class I3RecoPulseSeriesMapMaskTest2(unittest.TestCase):
    def setUp(self):
        super(I3RecoPulseSeriesMapMaskTest2, self).setUp()

        # create a pulse series map with a known bad
        # 0 width case. only two pulses.
        self.pulses = dataclasses.I3RecoPulseSeriesMap()
        key1 = icetray.OMKey(42, 7)
        vec = dataclasses.I3RecoPulseSeries()
        pulse = dataclasses.I3RecoPulse()
        pulse.time = 10226.6
        pulse.charge = 28.1466
        pulse.width = 0.833585
        vec.append(pulse)
        pulse.time = 10227.4
        pulse.charge = 18.5683
        pulse.width = 0.833585
        vec.append(pulse)
        self.pulses[key1] = vec

    def test_0_width(self):
        #print('before')
        #print(self.pulses)
        sdst = dataclasses.I3SuperDST(self.pulses)
        unpack = sdst.unpack()
        #print('----')
        #print('after')
        #print(unpack)
        if any(p.width == 0 for pulses in unpack.values() for p in pulses):
            raise Exception('0 width')

if __name__ == '__main__':
    unittest.main()
