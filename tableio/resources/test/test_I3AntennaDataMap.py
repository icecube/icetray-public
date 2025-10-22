#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2025 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

"""
Ensure that I3AntennaDataMap can be written correctly
"""

import os
import unittest
import tables
import numpy as np
from icecube import icetray, dataclasses, radcube, hdfwriter

def add_eventheader(frame):
    header = dataclasses.I3EventHeader()
    header.run_id = 1
    header.event_id = 1
    frame['I3EventHeader'] = header

def add_antmap(frame, nant, nchan, wvflen):
    waveforms = np.arange(nant * nchan * wvflen).reshape(nant, nchan, wvflen)
    antmap = dataclasses.I3AntennaDataMap()
    for iant in range(nant):
        chanmap = dataclasses.I3AntennaChannelMap()
        for ich in range(nchan):
            ts = dataclasses.AntennaTimeSeries()
            for i in waveforms[iant][ich]:
                ts.PushBack(float(i))
            ts.binning = 1.25
            ts.offset = iant * 100
            fftdata = dataclasses.FFTData()
            fftdata.LoadTimeSeries(ts)
            chanmap[ich] = dataclasses.I3AntennaChannel(fftdata)
        antmap[dataclasses.AntennaKey(1, iant+1)] = chanmap

    frame['I3AntennaDataMap'] = antmap

class TestAntennaDataMap(unittest.TestCase):
    def setUp(self):
        self.nant = 3
        self.nchan = 2
        self.wvflen = 1024
        self.waveforms = np.arange(self.nant * self.nchan * self.wvflen).reshape(self.nant, self.nchan, self.wvflen)

        self.filename = "foo.hdf5"
    
        # write an hdf file
        tray = icetray.I3Tray()
        tray.Add('I3InfiniteSource')
        tray.Add(add_eventheader, Streams=[icetray.I3Frame.DAQ])
        tray.Add("I3NullSplitter", "nullsplit")
        tray.Add(add_antmap, nant=self.nant, nchan=self.nchan, wvflen=self.wvflen)
        tray.Add(hdfwriter.I3HDFWriter,
            Output=self.filename,
            Keys=['I3AntennaDataMap'],
            Types=[],
            SubEventStreams=["nullsplit"])
        tray.Execute(1)

    def tearDown(self):
        os.unlink(self.filename)

    def testNodeExists(self):
        hdf = tables.open_file(self.filename)
        self.assertIsNotNone(hdf.get_node('/I3AntennaDataMap'))

    def testNumberOfRows(self):
        hdf = tables.open_file(self.filename)
        self.assertEqual(hdf.get_node('/I3AntennaDataMap').nrows, self.nant)

    def testProperties(self):
        hdf = tables.open_file(self.filename)
        node = hdf.get_node('/I3AntennaDataMap')
        self.assertTrue(np.all(node.cols.binning[:] == 1.25))
        self.assertTrue(np.all(node.cols.nbins[:] == 1024))
        self.assertEqual(node.cols.offset[-1], (self.nant-1) * 100)

    def testTimeSeries(self):
        hdf = tables.open_file(self.filename)
        node = hdf.get_node('/I3AntennaDataMap')
        ts0 = node.cols.waveform_ch0[:]
        ts1 = node.cols.waveform_ch1[:]
        self.assertEqual(ts0.shape, (self.nant, self.wvflen))
        self.assertEqual(ts1.shape, (self.nant, self.wvflen))
        self.assertEqual(list(node.cols.antenna[:]), list(np.arange(1, self.nant+1)))
        ts0 = ts0.reshape(self.nant, self.wvflen)
        ts1 = ts1.reshape(self.nant, self.wvflen)
        self.assertTrue(np.all(ts0[0] == self.waveforms[0][0][:]))
        self.assertTrue(np.all(ts1[-1] == self.waveforms[-1][1][:]))

if __name__ == '__main__':
    unittest.main()
