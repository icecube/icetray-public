#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import os
import unittest
from tempfile import NamedTemporaryFile

from icecube import dataclasses, dataio, icetray
from icecube.icetray import I3Units


# The test to run:
class DAQFrameMuxing(unittest.TestCase):
    def testPhysFrame(self):
        self.assertIn("I3Geometry", self.frame)
        self.assertIn("I3Calibration", self.frame)
        self.assertIn("I3DetectorStatus", self.frame)
        self.assertIn("InIceRawData", self.frame)
        self.assertEqual(len(self.frame["InIceRawData"]), 1)
        self.assertIn("DrivingTime", self.frame)
        self.assertIn("I3EventHeader", self.frame)
        self.assertEqual(frame["I3EventHeader"].sub_event_id, 1)


# Manufacture a file.
with NamedTemporaryFile(delete=False) as t:
    fname = t.name
    f = dataio.I3File(fname, "w")

    the_time = dataclasses.I3Time()
    the_time.set_utc_cal_date(1919, 1, 15, 0, 0, 0, 0)
    geo = dataclasses.I3Geometry()
    geo.start_time = the_time - 100
    geo.end_time = the_time + 100
    calib = dataclasses.I3Calibration()
    calib.start_time = the_time - 100
    calib.end_time = the_time + 100
    status = dataclasses.I3DetectorStatus()
    status.start_time = the_time - 100
    status.end_time = the_time + 100
    frame = icetray.I3Frame(icetray.I3Frame.Geometry)
    frame['I3Geometry'] = geo
    f.push(frame)

    frame = icetray.I3Frame(icetray.I3Frame.Calibration)
    frame['I3Calibration'] = calib
    f.push(frame)

    frame = icetray.I3Frame(icetray.I3Frame.DetectorStatus)
    frame['I3DetectorStatus'] = status
    f.push(frame)

    frame = icetray.I3Frame(icetray.I3Frame.DAQ)
    dlsm = dataclasses.I3DOMLaunchSeriesMap()
    dlsm[icetray.OMKey(7, 42)] = dataclasses.I3DOMLaunchSeries()
    frame['InIceRawData'] = dlsm
    t = dataclasses.I3Time()
    t.set_utc_cal_date(1919, 1, 15, 0, 0, 0, 0)
    frame['DrivingTime'] = t
    header = dataclasses.I3EventHeader()
    header.run_id = 7
    header.event_id = 42
    header.sub_event_id = 0
    header.start_time = t
    header.end_time = t + 10 * I3Units.microsecond
    frame['I3EventHeader'] = header
    f.push(frame)

    frame = icetray.I3Frame(icetray.I3Frame.Physics)  # a mostly-empty Physics frame.
    frame['DrivingTime'] = t + 2
    header.sub_event_id = 1
    frame['I3EventHeader'] = header
    f.push(frame)
    f.close()

    # Check I3Reader.
    tray = icetray.I3Tray()
    tray.Add('I3Reader', filename=fname)
    tray.Add(icetray.I3TestModuleFactory(DAQFrameMuxing))
    tray.Execute()

    if os.path.exists(fname):
        os.unlink(fname)
