#!/usr/bin/env python

#
#  Run frames on different streams through a reader/writer tray, verify that extra
#  cruft doesn't get in to the files
#
from icecube import icetray, dataclasses, dataio
import sys

i3f = dataio.I3File("alphabet.i3", dataio.I3File.Mode.Writing)

try:
    for frame in i3f:
        print frame
except:
    print "ok, that threw as expected"
    sys.exit(0)
else:
    print "FAIL: that should have thrown"
    sys.exit(1)
