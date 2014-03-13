#!/usr/bin/env python

from I3Tray import I3Tray
from icecube import dataio
import os

tray = I3Tray()

tray.AddModule("BottomlessSource", "bs")

# no If
def f1(frame):
    pass
    
tray.AddModule(f1, 'f1')

# a basic If
def f2(frame):
    pass
    
tray.AddModule(f2, 'f2',
               If = lambda fr:True)

# some other args and an If
def f3(frame, x='x', y='y'):
    assert x == 'configured'
    assert y == 'configured'
    
tray.AddModule(f3, 'f3',
               x = 'configured',
               y = 'configured',
               If = lambda fr:True)

tray.AddModule("TrashCan", "tc")

tray.Execute(1)
tray.Finish()
