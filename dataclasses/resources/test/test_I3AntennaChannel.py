#!/usr/bin/env python3

import os
import unittest
from icecube.dataio import I3File
from icecube.dataclasses import FFTData, I3AntennaChannel, AntennaTimeSeries
from I3Tray import *

theFileName = "TheFile.i3.gz"

class TestI3AntennaChannel(unittest.TestCase):

  def test_internal_variables(self):
    timeSeries = AntennaTimeSeries()
    for i in range(20):
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
    self.assertEqual(fftData.GetFrequencySpectrum(), fftData2.GetFrequencySpectrum())

  def test_serialize(self):
    frameName = "FrameEntry"

    timeSeries = AntennaTimeSeries()
    for i in range(20):
      timeSeries.PushBack(1)

    timeSeries.binning = 1
    timeSeries.offset = 0
    timeSeries[5] = 1000

    fftData = FFTData()
    fftData.LoadTimeSeries(timeSeries)

    chData = I3AntennaChannel(fftData)

    #Put into a file
    theI3File = I3File(theFileName, 'w')
    frame = icetray.I3Frame(icetray.I3Frame.DAQ)
    frame[frameName] = chData
    theI3File.push(frame)
    theI3File.close()

    #Open the new file and assert that it is equal
    newI3File = I3File(theFileName, 'r')
    testFrame = newI3File.pop_frame()

    readIn = testFrame[frameName]
    
    self.assertEqual(readIn.GetFFTData().GetTimeSeries(), fftData.GetTimeSeries())
    self.assertEqual(readIn.GetFFTData().GetFrequencySpectrum(), fftData.GetFrequencySpectrum())

  def tearDown(self):
    if os.path.exists(theFileName):
      os.remove(theFileName)
    self.assertEqual(os.path.exists(theFileName), False, "The file did not get removed in the cleanup!")




unittest.main()