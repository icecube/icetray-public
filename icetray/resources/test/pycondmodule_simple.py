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
    def __init__(self, context):
        I3ConditionalModule.__init__(self, context)
        
    def Configure(self):
        pass
    
    def Process(self):
        assert False

tray.AddModule(Mod, "PythonConditionalModule",
               If = lambda f: False)



tray.Execute(5)

print("Okay. Exiting.")
