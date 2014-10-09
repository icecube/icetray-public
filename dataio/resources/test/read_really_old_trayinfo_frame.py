#!/usr/bin/env python
#
from icecube import icetray, dataio
from os.path import expandvars

import os
import sys

testdata = expandvars("$I3_TESTDATA")
runfile = testdata + "/2006data/Run00089508.i3.gz"

i3file = dataio.I3File(runfile)

frame = i3file.pop_frame()

# the second is the really old one.  Shouldn't crash.
frame = i3file.pop_frame()

print("old frame: %s" % frame)


