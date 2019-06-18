#!/usr/bin/env python

# This is an example script that illustrates the problem in ticket #967
# http://code.icecube.wisc.edu/projects/icecube/ticket/967
#
# We only allow getting const objects from the frame to prevent exactly
# this problem.  What, I think, many people don't know is that under
# the hood we cast away the const-ness when passing references to python
# modules and functions.
#
# Therefore on the C++ side an I3Module could cache a pointer to a const
# object it got from the frame and expect that to never change, but an
# evil python module can come along and make all the changes it wants 
# to frame objects.
#
# What's currently recommended is to deep-copy frame objects in python
# if you want to modify them.  This isn't really safe, since it's easy
# to unwittingly change frame object that C++ might expect to be const.
# 
# Therefore I think the default should be to copy frame objects.  If you
# care about the performance hit and promise you won't change frame objects
# then we can provide a "fast_get" that casts away the const...or maybe
# there's an even better solution.  In any case, this is the problem.
#
# When this is fixed we should move this over to 'resources/tests'.


from os.path import expandvars
from I3Tray import I3Tray
from icecube import icetray, dataclasses, dataio

def evil_function(frame):
    frame["I3Geometry"].omgeo[icetray.OMKey(21,30)].omtype = \
        dataclasses.I3OMGeo.IceTop

tray = I3Tray()

GCDFN = expandvars("$I3_TESTDATA/GCD/GeoCalibDetectorStatus_2013.56429_V1.i3.gz")
tray.Add("I3InfiniteSource", prefix = GCDFN)
tray.Add(evil_function, streams = [icetray.I3Frame.DAQ])
tray.Add("TestConstCache")
tray.Execute(10)


