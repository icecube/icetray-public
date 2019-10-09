#!/usr/bin/env python

from I3Tray import I3Tray
from icecube import icetray 

class GCDQP(icetray.I3Module):
    def __init__(self, context):
        icetray.I3Module.__init__(self, context)

    def Geometry(self, frame):
        frame['I3Geometry'] = icetray.I3Bool(True)
        
    def Calibration(self, frame):
        frame['I3Calibration'] = icetray.I3Bool(True)
        
    def DetectorStatus(self, frame):
        frame['I3DetectorStatus'] = icetray.I3Bool(True)
        
    def DAQ(self, frame):
        frame['DAQyObject'] = icetray.I3Bool(True)
        
    def Physics(self, frame):
        frame['FizzyObject'] = icetray.I3Bool(True)

tray = I3Tray()

# generate empty frames
tray.Add("BottomlessSource")
tray.AddModule(GCDQP)

tray.Add("FrameCheck",
         ensure_geometry_has = ['I3Geometry'],
         ensure_calibration_has = ['I3Calibration'],
         ensure_detectorstatus_has = ['I3DetectorStatus'],
         ensure_daq_has = ['DAQyObject'],
         ensure_physics_has = ['FizzyObject'])               

tray.Execute(5)

