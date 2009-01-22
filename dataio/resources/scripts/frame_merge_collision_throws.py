#!/usr/bin/env python

#
#  Run frames on different streams through a reader/writer tray, verify that extra
#  cruft doesn't get in to the files
#
from icecube import icetray, dataclasses, dataio
from I3Tray import I3Tray
import sys

a = icetray.I3Frame(icetray.I3Frame.Physics)
b = icetray.I3Frame(icetray.I3Frame.Physics)

a['foo'] = icetray.I3Int(7)
b['foo'] = icetray.I3Int(8)

try:
    b.merge(a)
except:
    print "ok, that threw"
else:
    print "*** FAIL should have thrown"
    sys.exit(1)

try:
    a.merge(b)
except:
    print "ok, that threw"
else:
    print "*** FAIL should have thrown"
    sys.exit(1)

