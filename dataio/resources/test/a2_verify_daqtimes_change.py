#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from icecube import icetray, dataclasses, dataio
from icecube.icetray import I3Tray
from os.path import expandvars

tray = I3Tray()

runfile = expandvars("$I3_TESTDATA/2006data/Run00089508.i3.gz")

tray.AddModule("I3Reader", "read", Filename = runfile)

def SelectDaq(frame):
    assert 'DrivingTime' in frame
    dt = frame.Get("DrivingTime")
    utcdaq = dt.utc_daq_time
    print(utcdaq)
    assert SelectDaq.daqtime != utcdaq
    print("Yup prev time %lu different than new time %lu" % (SelectDaq.daqtime, utcdaq))
    SelectDaq.daqtime = utcdaq
    return True

SelectDaq.daqtime = 0

tray.AddModule(SelectDaq,"pickdaq",
               Streams = [icetray.I3Frame.Physics])

tray.AddModule("Dump","dump")


tray.Execute()

