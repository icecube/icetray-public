#!/usr/bin/env python3

import os
import unittest
from tempfile import NamedTemporaryFile

from icecube import dataclasses, dataio, icetray, phys_services
from icecube.icetray import I3Units

# The test to run:
phys_frames = 0
max_phys_frames = 3


class DAQFrameSplitting(unittest.TestCase):
    def testPhysFrame(self):
        global phys_frames
        self.assertIn("I3Geometry", self.frame)
        self.assertIn("I3Calibration", self.frame)
        self.assertIn("I3DetectorStatus", self.frame)
        self.assertIn("InIceRawData", self.frame)
        self.assertEqual(len(self.frame["InIceRawData"]), 1)
        self.assertIn("DrivingTime", self.frame)
        self.assertIn("I3EventHeader", self.frame)
        self.assertEqual(self.frame["I3EventHeader"].sub_event_stream, 'splitter')
        self.assertEqual(self.frame["I3EventHeader"].sub_event_id, phys_frames)
        self.assertEqual(self.frame["I3EventHeader"].sub_event_id, self.frame["TriggerID"].value)
        self.assertEqual(self.frame["I3EventHeader"].event_id, frame["I3EventHeader"].event_id)
        self.assertEqual(self.frame["I3EventHeader"].run_id, frame["I3EventHeader"].run_id)
        phys_frames += 1

    def Finish(self):
        self.assertEqual(phys_frames, max_phys_frames)


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
    f.close()

    class FrameSplitter(icetray.I3Module, phys_services.I3Splitter):
        def __init__(self, context):
            icetray.I3Module.__init__(self, context)
            phys_services.I3Splitter.__init__(self, self.configuration)
            self.AddOutBox('OutBox')

        def Configure(self):
            pass

        def DAQ(self, frame):
            print('DAQ Frame')
            self.PushFrame(frame)
            for i in range(0, max_phys_frames):
                subframe = self.get_next_sub_event(frame)
                subframe['TriggerID'] = icetray.I3Int(i)
                self.PushFrame(subframe)
            return True

    tray = icetray.I3Tray()
    tray.AddModule('I3Reader', 'reader', filename=fname)
    tray.AddModule(FrameSplitter, 'splitter')
    tray.AddModule(icetray.I3TestModuleFactory(DAQFrameSplitting), 'testy')
    tray.Execute()

    if os.path.exists(fname):
        os.unlink(fname)
