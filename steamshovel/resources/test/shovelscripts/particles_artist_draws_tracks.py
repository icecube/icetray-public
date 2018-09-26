from icecube.icetray import I3Frame
from icecube import dataclasses as dcl
import os
import sys
from time import sleep
scenario = window.gl.scenario

# open a test file so that we have a valid frame
test_file = os.environ["I3_TESTDATA"] + \
    "/event-viewer/Level3aGCD_IC79_EEData_Run00115990_slim.i3"

app.files.openLocalFile( test_file )
while app.files.framesAreProcessed:
    sleep( 0.1 )

# steamshovel jumps to the first Physics frame automatically

window.gl.perspectiveView = False
window.gl.cameraLoc = ( 0, -2000, 0 )
window.gl.cameraPivot = ( 500, 0, 0 )
window.gl.cameraOrientation = (1.0, 1.0, 0.0)

# add some extra particles
for i, shape in enumerate(( dcl.I3Particle.StartingTrack,
                            dcl.I3Particle.StoppingTrack,
                            dcl.I3Particle.ContainedTrack )):
    p = dcl.I3Particle()
    p.pos = dcl.I3Position( 100 + 300 * i, 0, 0 )
    p.dir = dcl.I3Direction( 0, 0, 1 )
    p.shape = shape
    if shape == dcl.I3Particle.ContainedTrack:
        p.length = 300
    p.time = 10000

    key = str(shape)
    frame[key] = p
    artist = scenario.add( "Particles", [key] )

file_name = "particles_artist_draws_tracks.png"
window.gl.screenshot( 200, 200, file_name )
