#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import unittest
from icecube.dataclasses import *
import numpy as np

def FillTimeSeriesWithData(timeSeries, n, dt):
  timeSeries.Clear()
  timeSeries.binning = dt
  for i in range(n):
    val = np.sin(2 * np.pi * i / (1. * n))
    timeSeries.PushBack(val)

def FillTimeSeriesWithData3d(timeSeries, n, dt):
  timeSeries.Clear()
  timeSeries.binning = dt
  for i in range(n):
    val = np.sin(2 * np.pi * i / (1. * n))
    timeSeries.PushBack(I3Position(val, val, val))


class TestHilbertTools(unittest.TestCase):

  def test_Hilbert_peak(self):
    timeSeries = AntennaTimeSeries()
    timeSeries.offset = -33
    FillTimeSeriesWithData(timeSeries, 10, 0.3)

    timeSeries[2] = 100
    peakTime = fft.GetHilbertPeakTime(timeSeries)
    maxVal = fft.GetHilbertPeak(timeSeries)
    self.assertAlmostEqual(peakTime, (2 * 0.3) - 33, 5)
    self.assertAlmostEqual(maxVal, 100, 3)

  def test_Hilbert_peak_3d(self):
    timeSeries = EFieldTimeSeries()
    timeSeries.offset = -33
    FillTimeSeriesWithData3d(timeSeries, 10, 0.3)

    timeSeries[2] = I3Position(100 / np.sqrt(3.), 100 / np.sqrt(3.), 100 / np.sqrt(3.))
    peakTime = fft.GetHilbertPeakTime(timeSeries)
    maxVal = fft.GetHilbertPeak(timeSeries)
    self.assertAlmostEqual(peakTime, (2 * 0.3) - 33, 5)
    self.assertAlmostEqual(maxVal, 100, 2)

  def test_wrappers(self):
    timeSeries = AntennaTimeSeries()
    timeSeries.offset = -33
    FillTimeSeriesWithData(timeSeries, 10, 0.3)
    fftData = FFTData()
    fftData.LoadTimeSeries(timeSeries)

    self.assertAlmostEqual(fft.GetHilbertPeak(timeSeries), fft.GetHilbertPeak(fftData), 5)
    self.assertAlmostEqual(fft.GetHilbertPeakTime(timeSeries), fft.GetHilbertPeakTime(fftData), 5)

  def test_wrappers_3d(self):
    timeSeries = EFieldTimeSeries()
    timeSeries.offset = -33
    FillTimeSeriesWithData3d(timeSeries, 10, 0.3)
    fftData = FFTData3D()
    fftData.LoadTimeSeries(timeSeries)

    self.assertAlmostEqual(fft.GetHilbertPeak(timeSeries), fft.GetHilbertPeak(fftData), 5)
    self.assertAlmostEqual(fft.GetHilbertPeakTime(timeSeries), fft.GetHilbertPeakTime(fftData), 5)

unittest.main()
