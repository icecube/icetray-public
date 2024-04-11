#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import unittest
from tempfile import NamedTemporaryFile

from icecube import icetray
from icecube.dataclasses import AntennaTimeSeries, FFTData, I3AntennaChannel
from icecube.dataio import I3File


class TestI3AntennaChannel(unittest.TestCase):
    def test_internal_variables(self):
        timeSeries = AntennaTimeSeries()
        for _i in range(20):
            timeSeries.PushBack(1)

        timeSeries.binning = 1
        timeSeries.offset = 0
        timeSeries[5] = 1000

        fftData = FFTData()
        fftData.LoadTimeSeries(timeSeries)

        chData = I3AntennaChannel(fftData)

        peak = chData.GetMaxAmplitude()
        peakTime = chData.GetTimeAtMaximum()
        fftData2 = chData.GetFFTData()

        self.assertAlmostEqual(peak, 1000, 5)
        self.assertAlmostEqual(peakTime, 5, 5)
        self.assertEqual(fftData.GetTimeSeries(), fftData2.GetTimeSeries())
        self.assertEqual(
            fftData.GetFrequencySpectrum(), fftData2.GetFrequencySpectrum()
        )

    def test_serialize(self):
        frameName = "FrameEntry"

        timeSeries = AntennaTimeSeries()
        for _i in range(20):
            timeSeries.PushBack(1)

        timeSeries.binning = 1
        timeSeries.offset = 0
        timeSeries[5] = 1000

        fftData = FFTData()
        fftData.LoadTimeSeries(timeSeries)

        chData = I3AntennaChannel(fftData)

        # Put into a file
        with NamedTemporaryFile() as t:
            theFileName = t.name

            theI3File = I3File(theFileName, "w")
            frame = icetray.I3Frame(icetray.I3Frame.DAQ)
            frame[frameName] = chData
            theI3File.push(frame)
            theI3File.close()

            # Open the new file and assert that it is equal
            newI3File = I3File(theFileName, "r")
            testFrame = newI3File.pop_frame()
            theI3File.close()

        readIn = testFrame[frameName]

        self.assertEqual(readIn.GetFFTData().GetTimeSeries(), fftData.GetTimeSeries())
        self.assertEqual(
            readIn.GetFFTData().GetFrequencySpectrum(), fftData.GetFrequencySpectrum()
        )


if __name__ == "__main__":
    unittest.main()
