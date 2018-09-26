from icecube import icetray
import os
import sys
from time import sleep
from __main__ import frame

# open a test file so that we have a valid frame
test_file = os.environ["I3_TESTDATA"] + \
    "/event-viewer/Level3aGCD_IC79_EEData_Run00115990_slim.i3"

app.files.openLocalFile( test_file )

# wait for index generation
while app.files.framesAreProcessed:
    sleep( 0.1 )

# check frames
expected_nkeys = [ 1, 13, 114, 13, 130, 13, 114, 13 ]
if len(expected_nkeys) != app.files.nFrames:
	icetray.i3logging.log_fatal( "wrong number of frames" )
for idx in range(app.files.nFrames):
	app.files.selectFrame(idx)
	if expected_nkeys[idx] != len(frame):
		icetray.i3logging.log_fatal( "wrong number of keys" )
