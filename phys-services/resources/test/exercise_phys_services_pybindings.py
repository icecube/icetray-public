#!/usr/bin/env python

from icecube import icetray, dataclasses, phys_services
from icecube.icetray.I3Test import *
import os

print("Working out I3XMLOMKey2MBID service")

infile = './phys-services/resources/mainboard_ids.xml.gz'
ms = phys_services.I3XMLOMKey2MBID(infile)
ambid = ms.get_mbid(icetray.OMKey(2,2))
aom = ms.get_omkey(ambid)

ENSURE(aom == icetray.OMKey(2,2), "Failed to find the right om in I3XMLOMKey2MBID")

print("Working out I3Calculator")

part1 = dataclasses.I3Particle()
part1.dir = dataclasses.I3Direction(1.0, 0.0, 0.0)
part2 = dataclasses.I3Particle()
part2.dir = dataclasses.I3Direction(0.0, 1.0, 0.0)
anglediff = phys_services.I3Calculator.angle(part1,part2)

print("Particle 1: %s" % part1.dir)
print("Particle 2: %s" % part2.dir)
print("Angle diff: %.2f degrees" % (anglediff/icetray.I3Units.deg))

print("Working out CutValues")

## This is just a dumb container.
cv = phys_services.I3CutValues()
cog_pos = dataclasses.I3Position(0.0,0.0,0.0)
cv.cog = cog_pos
cv.ldir = 500.5*icetray.I3Units.m
cv.ndir = 5
cv.nchan = 55


print("your I3CutValues: %s %.2f %d %d" % (cv.cog, cv.ldir, cv.ndir, cv.nchan))

print("Working out GCDFileService")

mytime = dataclasses.I3Time(2010, 158082172000000000)

infile = os.path.expandvars("$I3_TESTDATA/GCD/GeoCalibDetectorStatus_IC86.55697_corrected_V2.i3.gz")

my_fs = phys_services.I3GCDFileCalibrationService(infile)

my_cal = my_fs.get_calibration(mytime)
print("I found calibrations for %d DOMs" % len(my_cal.dom_cal))

print("Working out randoms")

rng = phys_services.I3GSLRandomService(31334)

print("Please take these 100 rands")
print([rng.gaus(0,1) for x in range(100)])



