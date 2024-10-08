#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from icecube.icetray import I3Tray
from os.path import expandvars
from icecube import icetray,dataclasses,dataio,phys_services

amageofile = expandvars("$I3_SRC/phys-services/resources/amanda.geo")
icecubegeofile = expandvars("$I3_SRC/phys-services/resources/icecube.geo")

tray = I3Tray()

tray.AddModule("I3InfiniteSource","streams",stream=icetray.I3Frame.Physics)
def set_time(fr):
    fr['DrivingTime'] = dataclasses.I3Time(2006, 0)
tray.AddModule(set_time, 'time')

tray.AddService("I3TextFileGeometryServiceFactory","geometry")(
              ("AmandaGeoFile",amageofile),
              ("IceCubeGeoFile",icecubegeofile),
              )

tray.AddModule("I3MetaSynth","muxme")

tray.AddModule("Dump","dump")

tray.AddModule("FrameCheck","check")(
    ("ensure_physics_has", ["I3Geometry", "DrivingTime"])
    )



tray.Execute(2)

