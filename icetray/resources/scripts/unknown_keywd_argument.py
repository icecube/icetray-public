#!/usr/bin/env python

from I3Tray import *

tray = I3Tray()


tray.AddModule("BottomlessSource","bs")

# can't convert that return value to bool or none
def f(frame, foo='foo', bar='bar', baz='baz'):
    print foo, bar, baz
    assert foo == 'foo', 'foo didnt get set'
    assert bar == 'BARWASSET', 'bar didnt get set'
    assert baz == 'baz', 'baz didnt get set'

    return True


tray.AddModule(f,"keywordfn",
               bar = 'BARWASSET',
               argdoesntexist = 'oopsie')

tray.AddModule('TrashCan', 'tc')

try:
    tray.Execute(1)
    print "That should have thrown a typeerror"
    sys.exit(1)

except TypeError, e:
    print e
    print "OK, threw as expected"

    

                   

