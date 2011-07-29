#!/usr/bin/env python

from I3Tray import *
from icecube import icetray, dataclasses, phys_services

class UseRandom(icetray.I3Module):
    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        self.AddParameter("I3RandomService", "the service", None)
        self.AddOutBox("OutBox")

    def Configure(self):
        self.rs = self.GetParameter("I3RandomService")

    def Physics(self, frame):

        if self.rs:
            rnd = self.rs.gaus(0,1)
            
        self.PushFrame(frame)

try:
    tray = I3Tray()

    # generate empty frames
    tray.AddModule("BottomlessSource","bottomless")

    tray.AddModule(UseRandom,"noservice")
    
    tray.AddModule(UseRandom,"service",
                   I3RandomService = phys_services.I3GSLRandomService(3141579)  )
    
    tray.AddModule(UseRandom,"explicit",
                   I3RandomService = None)

    tray.AddModule("ContextFreeServiceTestModule","livefreeordie")

    tray.AddModule("TrashCan","adios")

    tray.Execute(11)
    tray.Finish()
except StandardError, t:
    print t
    print "FAIL!!!"
    sys.exit(1)
else:
    # this should run without throwing
    sys.exit(0)
