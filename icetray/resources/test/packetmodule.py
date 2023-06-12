#!/usr/bin/env python3
#
# Sample i3module in python 
#

from icecube.icetray import I3Frame, I3Int, I3Module, I3PacketModule
from icecube.icetray import I3Tray

tray = I3Tray()

ndaq = 5
npackets = 0

# generate empty frames
tray.AddModule("BottomlessSource", Stream=I3Frame.DAQ)

class SimpleSplit(I3Module):
    def __init__(self, context):
        I3Module.__init__(self, context)

    def DAQ(self, fr):
        self.PushFrame(fr)
        self.PushFrame(I3Frame(I3Frame.Physics))

tray.Add(SimpleSplit)

class Mod(I3PacketModule):
    def __init__(self, context):
        I3PacketModule.__init__(self, context, I3Frame.DAQ)

    def FramePacket(self, frames):
        i = I3Int(len(frames) - 1)
        frames[0].Put("NSplits", i)
        for fr in frames:
            self.PushFrame(fr)
        global npackets
        npackets += 1

tray.Add(Mod)

# print em
tray.Add("Dump")

def test(fr):
	print('Frame has %d splits.' % fr["NSplits"].value)
	assert fr["NSplits"].value == 1
tray.Add(test, Streams=[I3Frame.DAQ])

tray.Execute(ndaq)


assert npackets == ndaq
