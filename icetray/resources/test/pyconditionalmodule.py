#!/usr/bin/env python
#
# Sample i3module in python 
#

from icecube.icetray import *
from I3Tray import *

tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource","bottomless")

n_should_pass = 17

def int_putter(frame):
    frame['int'] = I3Int(int_putter.index)
    int_putter.index += 1
    print("*** int_putter puts frame with %d" % frame['int'].value)
int_putter.index = 0

tray.AddModule(int_putter, "int_putter")

def the_condition(frame):
    return frame['int'].value % 2 == 0

class Mod(I3ConditionalModule):
    def __init__(self, context):
        I3ConditionalModule.__init__(self, context)
        self.nseen = 0
        
    def Configure(self):
        pass
    
    def Physics(self, frame):
        print("Value= %d" % frame['int'].value)
        assert the_condition(frame)
        self.nseen += 1
        self.PushFrame(frame)

    def Finish(self):
        global n_should_pass
        print("saw %d, should have seen %d" % (self.nseen, n_should_pass))
        assert self.nseen == n_should_pass

tray.AddModule(Mod, "PythonConditionalModule",
               If = the_condition) 

# print em
tray.AddModule("Dump","Dump")
# throw em out


# do it 5 times.
tray.Execute(n_should_pass * 2)
