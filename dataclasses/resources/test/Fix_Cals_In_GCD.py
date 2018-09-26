#!/usr/bin/env python

import sys, os
from os.path import expandvars

from I3Tray import *
from icecube import icetray, dataclasses, dataio

## Fake bad doms list, you can get real ones from BadDomList prject
badOMs = [OMKey(5,5),OMKey(5,6),OMKey(6,11),OMKey(18,45),OMKey(18,46),OMKey(19,59),
	  OMKey(40,52),OMKey(44,25),OMKey(44,26),OMKey(44,47),OMKey(44,48)]
print(badOMs)

INFILE = expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_IC80_DC6.54655.i3.gz")

OUTFILE="./GeoCalibDetectorStatus_IC86_corrected.i3.gz"


infile = dataio.I3File(INFILE)

geo_frame = infile.pop_frame()
while not geo_frame.Has('I3Geometry'): geo_frame = infile.pop_frame()
geometry = geo_frame.Get('I3Geometry')

cal_frame = infile.pop_frame()
while not cal_frame.Has('I3Calibration'): cal_frame = infile.pop_frame()
calibration = cal_frame.Get('I3Calibration')

status_frame = infile.pop_frame()
while not status_frame.Has('I3DetectorStatus'): status_frame = infile.pop_frame()
status = status_frame.Get('I3DetectorStatus')

dom_geo = geometry.omgeo
dom_cal = calibration.dom_cal
dom_status = status.dom_status
	
c_and_d_strings_to_check = [ 
	75, 76, 77, 78, 68, 69, 70, 71, 72, 73,
	74, 60, 61, 62, 63, 64, 65, 66, 67, 52,
	53, 54, 55, 56, 57, 58, 59, 44, 45, 46,
	47, 48, 49, 50, 36, 83, 37, 38, 39, 40,
	26, 27, 28, 29, 30, 17, 18, 19, 20, 21,
	10, 11, 12, 13,  2,  3,  4,  5,  6,
	8,  9,  16, 25,  35, 85, 84, 82, 81, 86,
	43, 34, 24, 15,  23, 33, 42, 51, 32, 41
	]

low_noise_DOMs_l = [ icetray.OMKey(82,54),  icetray.OMKey(84,54),  icetray.OMKey(85,55)]

for e,p in dom_geo:

	if e not in badOMs and e in dom_cal and e in dom_status:				
		cal_this_om = dom_cal[e]
		status_this_om = dom_status[e]

		if e.om == 39 and e.string == 72 :
			if status_this_om.lc_mode != dataclasses.I3DOMStatus.LCMode.UpOrDown :
				status.dom_status[e].lc_mode = dataclasses.I3DOMStatus.LCMode.UpOrDown
				print(" %s correcting LCMode to %d" % ( str(e), int(dom_status[e].lc_mode) ))
				
		if float(cal_this_om.dom_cal_version[:3]) < 7.5 :
			print("Bad DOMCal")
			print('  %s  DOMCalVersion = %s' % (str(e), cal_this_om.dom_cal_version))
			calibration.dom_cal[e].dom_cal_version = '7.5.0'
			print('  correcting to %s' % calibration.dom_cal[e].dom_cal_version)
		
		threshold = dataclasses.spe_pmt_threshold(status_this_om,
							cal_this_om) / I3Units.mV

		if threshold < 0 :
			print('Pathological PMT discriminator threshold')
			print('  %s  threshold = %2.2f mV' % (str(e), threshold))
			fit = dataclasses.LinearFit()
			fit.slope = NaN
			fit.intercept = NaN
			calibration.domCal[e].pmt_disc_calib = fit
			print('  correcting to %2.2f mV' % \
			      (dataclasses.spe_pmt_threshold(status_this_om,calibration.dom_cal[e])/I3Units.mV))

		if e.string == 87 or e.string == 88 :
			print("There should be no string 87 or 88")
			# make a new omkey
			k = icetray.OMKey(e.string - 8,e.om)
			if k in dom_geo :
				print("Ooops!  It already exists!")
			print("   moving %s to %s" % (str(e),str(k)))
			# move the I3OMGeo
			dom_geo[k] = dom_geo[e]
			del dom_geo[e]
			# move the I3DOMCalibration
			dom_cal[k] = dom_cal[e]
			del dom_cal[e]
			# move the I3DOMStatus
			dom_status[k] = dom_status[e]
			del dom_status[e]			

		# check for unusually low noise DOMs that were incorrectly translated into the DB
		if e in low_noise_DOMs_l :
			noiseRate = calibration.dom_cal[e].dom_noise_rate
			if noiseRate < 400 * I3Units.hertz :
				calibration.dom_cal[e].dom_noise_rate = noiseRate + 1*I3Units.kilohertz
				print("  correcting noise from %fHz to %fHz in %s" % (noiseRate/I3Units.hertz, calibration.dom_cal[e].dom_noise_rate/I3Units.hertz,e))
				
			
del geo_frame['I3Geometry']
geo_frame['I3Geometry'] = geometry

del cal_frame['I3Calibration']
cal_frame['I3Calibration'] = calibration

del status_frame['I3DetectorStatus']
status_frame['I3DetectorStatus'] = status


outfile = dataio.I3File(OUTFILE, 'w')
outfile.push(geo_frame)
outfile.push(cal_frame)
outfile.push(status_frame)
outfile.close()

import os
os.unlink(OUTFILE)

