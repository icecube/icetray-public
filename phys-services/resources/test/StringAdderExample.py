#!/usr/bin/env python
from I3Tray import *
from os.path import expandvars
from icecube import icetray,dataclasses,dataio,phys_services

amageofile = expandvars("$I3_SRC/phys-services/resources/amanda.geo")
icecubegeofile = expandvars("$I3_SRC/phys-services/resources/icecube.geo")

tray = I3Tray()

tray.AddModule("I3InfiniteSource","streams",Stream=icetray.I3Frame.Physics)
def set_time(fr):
        fr['DrivingTime'] = dataclasses.I3Time(2006, 0)
tray.AddModule(set_time, 'time')
tray.AddService("I3TextFileGeometryServiceFactory","geometry")(
              ("AmandaGeoFile",amageofile),
              ("IceCubeGeoFile",icecubegeofile),
              )

tray.AddService("I3GeometrySelectorServiceFactory","geo_selector")(
    ("StringsToUse","21,29,39,38,30,40,49"),
    ("StationsToUse","21,29,39,38,30,40,49,50"),
    ("GeoSelectorName","IC9")
    )

tray.AddService("I3StringAdderServiceFactory","str_adder")(
    ("GeoServiceName","IC9"),
    ("GeoSelectorName","IC9++"),
    ("XPositions",[10.*I3Units.m,20.*I3Units.m,30.*I3Units.m]),
    ("YPositions",[15.*I3Units.m,25.*I3Units.m,35.*I3Units.m]),
    )

tray.AddService("I3GeometrySelectorServiceFactory","nother_geo_selector")(
    ("StringsToUse","21,29,39,38,30,40,49,83"),
    ("StationsToUse","21,29,39,38,30,40,49,50,83"),
    ("GeoServiceName","IC9++"),
    ("GeoSelectorName","IC9--")
    )

tray.AddModule("I3MetaSynth","muxme")(
   ("GeometryService","IC9--")
   )

tray.AddModule("I3Writer","writer")(
    ("filename","string_add_example.i3")
    )

tray.AddModule("Dump","dump")

tray.AddModule("FrameCheck","check")(
    ("ensure_physics_has", ["I3Geometry", "DrivingTime"])
    )



tray.Execute(10)


import os
os.remove("string_add_example.i3")

