#!/usr/bin/env python

from I3Tray import *
tray = I3Tray()

try:
    # generate empty frames
    tray.Add("BottomlessSource")
    tray.Add("AddNulls",
             where = ['foo'],
             If = ['this is', 'not a', 'function'])
    tray.Execute(10)

except:
    print("ok, at that threw as expected.")
    sys.exit(0)
else:
    print("that should have thrown")
    sys.exit(1)




