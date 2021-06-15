#!/usr/bin/env python

#
#  If an I3File is opened Writing it won't give you an iterator.
#
from icecube import icetray, dataclasses, dataio
import sys, os

fn = "alphabet2.i3"
i3f = dataio.I3File(fn, 'w')

try:
    for frame in i3f:
        print(frame)
except:
    print("ok, that threw as expected")
    sys.exit(0)
else:
    print("FAIL: that should have thrown")
    sys.exit(1)

if os.path.exists(fn):
    os.unlink(fn)
