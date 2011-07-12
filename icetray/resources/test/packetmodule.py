#!/usr/bin/env python
#
# Sample i3module in python 
#

from icecube.icetray import *
from I3Tray import *

tray = I3Tray()

ndaq = 5
npackets = 0

# generate empty frames
tray.AddModule("BottomlessSource","bottomless", Stream=I3Frame.DAQ)

class SimpleSplit(I3Module):
    def __init__(self, context):
        I3Module.__init__(self, context)
        self.AddOutBox("OutBox")
    def Configure(self):
        pass
    def DAQ(self, fr):
        self.PushFrame(fr)
        self.PushFrame(I3Frame(I3Frame.Physics))

tray.AddModule(SimpleSplit, 'split')

class Mod(I3PacketModule):
    def __init__(self, context):
        I3PacketModule.__init__(self, context)
        self.AddOutBox("OutBox")
    def Configure(self):
        pass
    def FramePacket(self, frames):
        i = icetray.I3Int(len(frames) - 1)
        frames[0].Put("NSplits", i)
        for fr in frames:
            self.PushFrame(fr)
	global npackets
        npackets += 1

tray.AddModule(Mod, "mod", PacketSentinelStream=I3Frame.DAQ)

# print em
tray.AddModule("Dump","dump")

def test(fr):
	print('Frame has %d splits.' % fr["NSplits"].value)
	assert fr["NSplits"].value == 1
tray.AddModule(test, "test", Streams=[I3Frame.DAQ])

# throw em out
tray.AddModule("TrashCan","adios")

tray.Execute(ndaq)
tray.Finish()

assert npackets == ndaq
