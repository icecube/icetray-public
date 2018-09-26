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
window.gl.cameraLoc = ( 0, -1500, 0 )
window.gl.cameraPivot = ( 500, 500, 500 )
window.gl.cameraOrientation = (1.0, 1.0, 0.0)

# add some extra particles
for i, beta in enumerate((0.7, 0.8, 0.9, 1.0)):
    p = dcl.I3Particle()
    p.pos = dcl.I3Position( 100 + 100 * i, 0, 0 )
    p.dir = dcl.I3Direction( 0, 0, 1 )
    p.type = dcl.I3Particle.MuPlus
    p.speed = beta * dcl.I3Constants.c
    p.time = 38000

    key = "p{:0.3f}".format(beta)
    frame[key] = p
    artist = scenario.add( "Particles", [key] )
    scenario.changeSetting(artist, "arrow head size", 50.0)
    scenario.changeSetting(artist, "Cherenkov cone size", 100.0)

scenario.add( "CoordinateSystem", [] )
window.gl.screenshot( 200, 200, "cherenkov_cone.png" )
