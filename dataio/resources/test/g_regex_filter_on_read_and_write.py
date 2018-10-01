#!/usr/bin/env python

from I3Tray import *

from os.path import expandvars

import os
import sys

from icecube import icetray 
from icecube import dataclasses 
from icecube import phys_services 
from icecube import dataio 

class PutParticle(icetray.I3Module):
    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        self.AddParameter("where", "Name of object in the frame,", "")
        self.AddOutBox("OutBox")
        
    def Configure(self) :
        self.where = self.GetParameter("where")

    def Physics(self, frame):
        frame[self.where] = dataclasses.I3Particle()
        self.PushFrame(frame)


tray = I3Tray()

tray.AddModule("I3Reader",
               Filename="pass1.i3",
               skipkeys=["IceTop.*"])

tray.AddModule("FrameCheck", 
    ensure_physics_has = ["DrivingTime", "I3EventHeader", "InIceRawData"],
    ensure_physics_hasnt = ["IceTopRawData", "IceTopRecoHitSeries"]
    )

#
# To verify filter-out of special chars
#
tray.AddModule(PutParticle, where = "particle?\w")

tray.AddModule("Dump","dump")

tray.AddModule("I3Writer", 
    filename = "filtered.i3",
    skipkeys = [".*Reco.*", "\w{4}where"]
    )

tray.Execute()


