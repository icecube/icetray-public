#!/usr/bin/python

from icecube import icetray, dataclasses, dataio, phys_services
from I3Tray import *
from os.path import expandvars

tray = I3Tray()


tools = expandvars("$I3_PORTS")
runfile = tools + "/test-data/2006data/Run00089508.i3.gz"

tray.AddService("I3ReaderServiceFactory", "readers",
		Filename = runfile)

tray.AddModule("I3Muxer", "mux")

def SelectDaq(frame):
	assert 'DrivingTime' in frame
	dt = frame.Get("DrivingTime")
	utcdaq = dt.GetUTCDaqTime()
	print utcdaq
	assert SelectDaq.daqtime != utcdaq
	print "Yup prev time %lu different than new time %lu" % (SelectDaq.daqtime, utcdaq)
	SelectDaq.daqtime = utcdaq
	return True

SelectDaq.daqtime = 0

tray.AddModule(SelectDaq,"pickdaq",
	       Streams = [icetray.I3Frame.Physics])

tray.AddModule("Dump","dump")

tray.AddModule("TrashCan","trash")
tray.Execute()
tray.Finish()
