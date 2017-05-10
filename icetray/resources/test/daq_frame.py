#!/usr/bin/env python

from icecube import icetray, dataclasses, dataio, phys_services
from icecube.icetray import I3Units
import I3Tray
import os, unittest

# The test to run:
class DAQFrameMuxing(unittest.TestCase):
	def testPhysFrame(self):
		self.assert_("I3Geometry" in self.frame)
		self.assert_("I3Calibration" in self.frame)
		self.assert_("I3DetectorStatus" in self.frame)
		self.assert_("InIceRawData" in self.frame)
		self.assert_(len(self.frame["InIceRawData"]) == 1)
		self.assert_("DrivingTime" in self.frame)
		self.assert_("I3EventHeader" in self.frame)
		self.assertEquals(frame["I3EventHeader"].sub_event_id, 1)

# Manufacture a file. 
fname = os.environ["I3_BUILD"] + "/daq_frame_test.i3.gz"
if os.path.exists(fname):
	os.unlink(fname)
the_time = dataclasses.I3Time()
the_time.set_utc_cal_date(1919, 1, 15, 0, 0, 0, 0)
f = dataio.I3File(fname, "w")
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
dlsm[icetray.OMKey(7,42)] = dataclasses.I3DOMLaunchSeries()
frame['InIceRawData'] = dlsm
t = dataclasses.I3Time()
t.set_utc_cal_date(1919, 1, 15, 0, 0, 0, 0)
frame['DrivingTime'] = t
header = dataclasses.I3EventHeader()
header.run_id = 7
header.event_id = 42
header.sub_event_id = 0
header.start_time = t
header.end_time = t+10*I3Units.microsecond
frame['I3EventHeader'] = header
f.push(frame)
frame = icetray.I3Frame(icetray.I3Frame.Physics) # a mostly-empty Physics frame.
frame['DrivingTime'] = t + 2
header.sub_event_id = 1
frame['I3EventHeader'] = header
f.push(frame)
f.close()

# Check I3Reader.
tray = I3Tray.I3Tray()
tray.Add('I3Reader', filename=fname)
tray.Add(icetray.I3TestModuleFactory(DAQFrameMuxing))
tray.Execute()

if os.path.exists(fname):
	os.unlink(fname)
