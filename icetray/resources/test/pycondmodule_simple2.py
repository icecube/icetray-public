#!/usr/bin/env python
#
# Sample i3module in python 
#

from icecube.icetray import *
from I3Tray import *

tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource","bottomless")

class Mod(I3ConditionalModule):
    count = 0
    def __init__(self, context):
        I3ConditionalModule.__init__(self, context)
        
    def Configure(self):
        pass
    
    def Process(self):
        Mod.count += 1
        f = self.PopFrame()
        self.PushFrame(f)
        print("process!")
        
tray.AddModule(Mod, "PythonConditionalModule",
               If = lambda f: True)



tray.Execute(5)

assert Mod.count == 5
print("Module executed 5 times as expected, okay.")
