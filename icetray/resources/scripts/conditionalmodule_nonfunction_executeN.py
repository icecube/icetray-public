#!/usr/bin/env python
#
# Simplest possible icetray script.
#

from I3Tray import *
tray = I3Tray()

try:
    tray.AddModule("BottomlessSource", "bs")

    # generate empty frames
    tray.AddModule("AddNulls","an",
                   where = ['foo'],
                   If = [1,2,3])

    tray.AddModule("TrashCan", "tc")

    print "About to execute    "
    # do it 5 times.
    tray.Execute(10)
    tray.Finish()


except:
    print "ok, at that threw as expected."

else:
    print "that should have thrown"
    sys.exit(1)




