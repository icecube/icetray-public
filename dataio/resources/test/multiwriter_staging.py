#!/usr/bin/env python3

import os
from icecube import icetray, dataclasses, dataio

def testfile(fname: str, project: str="dataio"):
	"""Quick-and-dirty clone of I3Test::testfile"""
	i3_work = os.environ.get('I3_BUILD', None)
	assert(i3_work is not None)
	dirname = i3_work + "/" + project + "/testdata"
	os.makedirs(dirname, exist_ok=True)
	return dirname + "/" + fname

someLargeObject=dataclasses.I3Calibration()
print(dir(someLargeObject))
for i in range(0,1000):
	someLargeObject.dom_cal[icetray.OMKey(i,10)]=dataclasses.I3DOMCalibration()

def do_multiwrite_test(suffix="", lenient=True):
	filenameTemplate=testfile("multi-%u.i3"+suffix)
	expectedFiles=4

	tray = icetray.I3Tray()
	tray.context["I3FileStager"] = dataio.get_stagers()
	tray.AddModule("I3InfiniteSource","Source")
	tray.AddModule(lambda frame: frame.Put("data", someLargeObject), streams=[icetray.I3Frame.DAQ])
	tray.AddModule("Dump","dump")
	tray.AddModule("I3MultiWriter","Writer")(
		("Filename","file://"+filenameTemplate),
		("SizeLimit",10),
		("Streams",[icetray.I3Frame.DAQ]))
	icetray.set_log_level(icetray.I3LogLevel.LOG_INFO)
	tray.Execute(expectedFiles)
	tray.Finish()

	frameCount = 0
	for i in range(0, expectedFiles):
		filename=filenameTemplate%i
		try:
			infile = dataio.I3File(filename,"read")
			fileFrameCount = 0
			while infile.more():
				infile.pop_frame()
				fileFrameCount+=1
			assert(fileFrameCount!=0)
			frameCount+=fileFrameCount
		except Exception as e:
			if not lenient:
				raise AssertionError("Failed to read " + filename + ": " + str(e))
			else:
				break
	print("Read back", frameCount, "frames")
	assert(expectedFiles == frameCount) #All frames must be read back

	for i in range(0, expectedFiles):
		filename=filenameTemplate%i
		try:
			os.remove(filename)
		except FileNotFoundError:
			break

do_multiwrite_test("")
do_multiwrite_test(".gz", lenient=True)
do_multiwrite_test(".bz2", lenient=True)
do_multiwrite_test(".zst")
