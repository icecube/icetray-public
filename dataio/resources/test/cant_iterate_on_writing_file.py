#!/usr/bin/env python

#
#  If an I3File is opened Writing it won't give you an iterator.
#
from icecube import icetray, dataclasses, dataio
import sys

i3f = dataio.I3File("alphabet.i3", dataio.I3File.Mode.Writing)

try:
    for frame in i3f:
        print(frame)
except:
    print("ok, that threw as expected")
    sys.exit(0)
else:
    print("FAIL: that should have thrown")
    sys.exit(1)
