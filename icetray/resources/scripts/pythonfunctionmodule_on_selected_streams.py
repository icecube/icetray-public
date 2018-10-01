#!/usr/bin/python

from icecube import icetray, dataclasses, dataio, util
from I3Tray import *
import os

tray = I3Tray()

#
#  Will issue frames on the 4 rotating streams
#
tray.AddModule(util.RoundRobinStreams, "bs",
               Streams = [icetray.I3Frame.Geometry,
                          icetray.I3Frame.DetectorStatus,
                          icetray.I3Frame.Calibration,
                          icetray.I3Frame.Physics]
               )

#
#  Run int-putter on only calibration and trayinfo frames
#
def putint(frame):
    frame['foo'] = icetray.I3Int(7)
    
tray.AddModule(putint, "putint",
               Streams = [icetray.I3Frame.Calibration,
                          icetray.I3Frame.Physics])

def printer(frame):
    print frame.GetStop()
    print frame

# tray.AddModule(printer, 'p')

#
#   Filter out frames the putint didn't see
#
tray.AddModule(lambda x: 'foo' in x, "filter",
               Streams = [icetray.I3Frame.Geometry,
                          icetray.I3Frame.DetectorStatus,
                          icetray.I3Frame.Calibration,
                          icetray.I3Frame.Physics])

tray.AddModule("Dump", "dump")
tray.AddModule("CountFrames", "count",
               Calibration = 2,
               Physics = 2,
               DetectorStatus = 0,
               Geometry = 0)
               

tray.AddModule("TrashCan", "tc")

tray.Execute(8)
tray.Finish()


    

