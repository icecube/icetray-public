#!/usr/bin/env python3

# Booking a I3<thing>SeriesMap has two options: bookGeometry, and bookToParticle.
# Make sure both of them (or one, or none) all work

from icecube import icetray, dataclasses, dataio, tableio
import os

import I3Tray

tray = I3Tray.I3Tray()

## Read a GCD, because we need the I3Geometry to use these extra options.
## And also a file with some pulses and tracks and stuff in it.
i3_testdata = os.path.expandvars("$I3_TESTDATA")
tray.AddModule("I3Reader", "readme",
    filenamelist=[i3_testdata+"/GCD/GeoCalibDetectorStatus_2016.57531_V0.i3.gz",
        i3_testdata+"/level3-filter-muon/Level2_IC86.2016_NuMu.021002.muonL3test.i3.zst"])

dirname = os.environ['I3_BUILD'] + '/tableio/recopulse_options_test'
tabler = tableio.I3CSVTableService(dirname)

p = "SplitInIcePulses"
t = "PoleMuonLlhFit"
key_none = dict(key = p,
    converter = dataclasses.converters.I3RecoPulseSeriesMapConverter(),
    name = p+"_none")
key_geo = dict(key = p,
    converter = dataclasses.converters.I3RecoPulseSeriesMapConverter(bookGeometry=True),
    name = p+"_withGeo")
key_track = dict(key = p,
    converter = dataclasses.converters.I3RecoPulseSeriesMapConverter(bookToParticle=t),
    name = p+"_withTrack")
key_geotrack = dict(key = p,
    converter = dataclasses.converters.I3RecoPulseSeriesMapConverter(bookGeometry=True, bookToParticle=t),
    name = p+"_withGeoTrack")

tray.AddModule(tableio.I3TableWriter, 'scribe',
	tableservice=tabler,
    keys=[key_none, key_geo, key_track, key_geotrack],
    subeventstreams=["InIceSplit"]
	)


tray.Execute(10)  # The first InIceSplit P-frame (event number 12) is the 10th frame.

## Remove the test results
import shutil
shutil.rmtree(dirname)

