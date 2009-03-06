#!/usr/bin/env python
#
# Sample i3module in python 
#

from icecube.icetray import *
from I3Tray import *

tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource","bottomless")

index = 0
n_should_pass = 25

def int_putter(frame):
    global index
    frame['int'] = I3Int(index)
    index += 1

tray.AddModule(int_putter, "putter")

class Mod(I3ConditionalModule):
    def __init__(self, context):
        I3ConditionalModule.__init__(self, context)
        self.nseen = 0
        
    def Configure(self):
        pass
    
    def Physics(self, frame):
        print "Value=", frame['int'].value
        assert frame['int'].value % 2 == 0
        self.nseen += 1
        self.PushFrame(frame)

    def Finish(self):
        global n_should_pass
        assert self.nseen == n_should_pass


tray.AddModule(Mod, "mod",
               If = lambda frame: frame['int'].value % 2 == 0) 

# print em
tray.AddModule("Dump","dump")
# throw em out
tray.AddModule("TrashCan","adios")

# do it 5 times.
tray.Execute(n_should_pass * 2)

# see ya.
tray.Finish()
