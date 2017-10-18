#!/usr/bin/env python

from I3Tray import I3Tray
from icecube import icetray
from icecube import dataio
import sys

tray = I3Tray()

tray.AddModule("BottomlessSource", "bs")

def frameloader():
    def impl(frame):
        frame['counter'] = icetray.I3Int(impl.i)
        impl.i += 1
    impl.i = 0
    return impl

tray.AddModule(frameloader(), "loader")

#
#  Only keep 'tag' if counter is even
#
tray.AddModule('Keep', 'k',
               If=lambda f: f['counter'].value % 3 == 0,
               Keys=['tag'])

def framecheck(frame):
    print(frame)
    if 'counter' in frame:
        assert frame['counter'].value % 3 != 0
            
tray.AddModule(framecheck, 'check')



tray.Execute(100)


