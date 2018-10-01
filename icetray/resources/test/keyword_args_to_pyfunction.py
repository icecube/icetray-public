#!/usr/bin/env python

from icecube.icetray import *
from I3Tray import *

tray = I3Tray()


tray.AddModule("BottomlessSource","bs")

# can't convert that return value to bool or none
def f(frame, foo='foo', bar='bar', baz='baz'):
    print('%s %s %s' % (foo, bar, baz))
    assert foo == 'foo', 'foo didnt get set'
    assert bar == 'BARWASSET', 'bar didnt get set'
    assert baz == 'baz', 'baz didnt get set'

    return True


tray.AddModule(f,"keywordfn",
               bar = 'BARWASSET')



tray.Execute(1)



                   

