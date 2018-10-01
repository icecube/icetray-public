#!/usr/bin/env python

from icecube import icetray
from I3Tray import *

tray = I3Tray()


tray.AddModule("BottomlessSource","bs")

def putints(frame, where, value):
    frame[where] = icetray.I3Int(value)
    print 'put value', value, 'at', where

tray.AddModule(putints, 'putter',
               where = 'here',
               value = 777)

def checkints(frame, where, value):
    i3i = frame[where]
    assert i3i.value == value
    print 'value', value, 'ok '
    
tray.AddModule(checkints, 'ci',
               where = 'here',
               value = 777)

tray.AddModule('Dump', 'dump')
tray.AddModule('TrashCan', 'tc')

tray.Execute(10)

    

                   

