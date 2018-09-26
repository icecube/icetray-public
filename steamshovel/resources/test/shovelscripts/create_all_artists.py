from __future__ import print_function
from icecube.icetray import I3Frame
import os
from time import sleep
scenario = window.gl.scenario

test_file = os.environ["I3_TESTDATA"] + \
	"/event-viewer/Level3aGCD_IC79_EEData_Run00115990.i3"

app.files.openLocalFile( test_file )
while app.files.framesAreProcessed:
	sleep(0.1)

# steamshovel jumps to the first Physics frame automatically

# define artists and their test keys
artist_config = {
	"AngleClock": (["MPEFit"],),
	"Bubbles": (["I3Geometry", "OfflinePulses"],
	            ["I3Geometry", "DOMs_OfflinePulses_FR_DCFid"],
	            ["I3Geometry", "InIceRawData"],
	            ["I3Geometry", "SplitGeo2"]),
	"ChargeBubbles": (["I3Geometry", "OfflinePulses"],
	                  ["I3Geometry", "SplitGeo2"]),
	"CoordinateSystem": ([],),
	"Detector": (["I3Geometry"],),
	"DOMLabel": (["I3Geometry"],),
	"DOMLaunchHistogram": (["InIceRawData"],),
	"DOMOrientation": (["I3Geometry"],),
	"Ice": ([],),
	# IceTopViewer: once without args, once with args
	"IceTopViewer":([],),
	"IceTopViewer":(["I3Geometry", "OfflinePulsesHLC"],),
	"Particles":(["MPEFit"],),
	"ParticleUncertainty":(["MPEFit_SLCParaboloidFitParams"],),
	"PhotonPaths":([],), # to-do: test with appropriate input
	"PlaneWave":(["MPEFit"],),
	"Position":(["MPEFit"],),
	"RecoPulseWaveform":(["OfflinePulses"],),
	"StaticImage":([],),
	"TextSummary":(["I3EventHeader"], 
	               ["FilterMask"],
	               ["DrivingTime"],
	               ["L3Cut_Qtot"]),
	"TimeLabel":([],),
	"UserLabel":([],),
	"Waveform":(["InIceRawData"],),
}

return_code = 0
for artist_name in scenario.listAvailableArtists():
	try:
		artist_keys_set = artist_config[artist_name]
	except KeyError:
		print( ("Error in create_all_artists.py: {0} has no configuration, "
		        "please add a valid configuration to make the test work")
		       .format(artist_name) )
		return_code = 1
		continue

	for artist_keys in artist_keys_set:
		try:
			scenario.add( artist_name, artist_keys )
		except Exception as e:
			print( "Error: {} raised {} with keys {}".format(artist_name, e, artist_keys) )
			return_code = 2

# shut down
app.quit(return_code)
