#!/usr/bin/env python

from I3Tray import I3Tray
from icecube import icetray 

class Source(icetray.I3Module):
    def __init__(self, context):
        icetray.I3Module.__init__(self, context)

        self.frame_types = [icetray.I3Frame.Physics,
                            icetray.I3Frame.DAQ,
                            icetray.I3Frame.DetectorStatus,
                            icetray.I3Frame.Calibration,
                            icetray.I3Frame.Geometry]        
        
    def Process(self):
        frame_type = self.frame_types.pop()
        self.PushFrame(icetray.I3Frame(frame_type))
        
class GCDQP(icetray.I3Module):
    def __init__(self, context):
        icetray.I3Module.__init__(self, context)

    def Geometry(self, frame):
        frame['I3Geometry'] = icetray.I3Bool(True)
        self.PushFrame(frame)
                
    def Calibration(self, frame):
        frame['I3Calibration'] = icetray.I3Bool(True)
        self.PushFrame(frame)
        
    def DetectorStatus(self, frame):
        frame['I3DetectorStatus'] = icetray.I3Bool(True)
        self.PushFrame(frame)
        
    def DAQ(self, frame):
        frame['DAQyObject'] = icetray.I3Bool(True)
        self.PushFrame(frame)
        
    def Physics(self, frame):
        frame['FizzyObject'] = icetray.I3Bool(True)
        self.PushFrame(frame)

tray = I3Tray()
    
tray.Add(Source)
tray.Add(GCDQP)

tray.Add("FrameCheck",
         ensure_geometry_has = ['I3Geometry'],
         ensure_calibration_has = ['I3Calibration'],
         ensure_detectorstatus_has = ['I3DetectorStatus'],
         ensure_daq_has = ['DAQyObject'],
         ensure_physics_has = ['FizzyObject'])               

tray.Execute(5)

del tray
