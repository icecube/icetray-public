#!/usr/bin/env python
from I3Tray import *
from os.path import expandvars
from icecube import icetray,dataclasses,dataio,phys_services

filename = expandvars("$I3_TESTDATA/GCD/GeoCalibDetectorStatus_IC86.55697_corrected_V2.i3.gz")

tray = I3Tray()

tray.AddService("I3GCDFileServiceFactory","geometry")(
              ("GCDFileName",filename)
              )

tray.AddService("I3GeometrySelectorServiceFactory","geo_selector")(
    ("StringsToUse","21,29,39,38,30,40,49"),
    ("StationsToUse","21,29,39,38,30,40,49,50"),
    ("GeoSelectorName","I3GeometrySelectorService")
    )


tray.AddModule("I3InfiniteSource","source",Stream=icetray.I3Frame.Physics)
def set_time(fr):
        fr['DrivingTime'] = dataclasses.I3Time(2006, 0)
tray.AddModule(set_time, 'time')
tray.AddModule('I3MetaSynth',"muxme")

tray.AddModule("Dump","dump")

tray.AddModule("FrameCheck","check")(
    ("ensure_physics_has", ["I3Geometry", "I3Calibration", "I3DetectorStatus","DrivingTime"])
    )



tray.Execute(4)

