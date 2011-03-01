#!/usr/bin/env python

#
#  AddOutBox works if in constructor
#

from I3Tray import *
tray = I3Tray()
from icecube.icetray import I3Module


class M(I3Module):
    def __init__(self, context):
        I3Module.__init__(self, context)
        self.AddOutBox('OutBox')
    def Configure(self):
        pass

    def Physics(self, frame):
        self.PushFrame(frame, "OutBox")

tray.AddModule("BottomlessSource","bottomless")
tray.AddModule(M,"do")
tray.AddModule("TrashCan","tc")

tray.Execute(1)


