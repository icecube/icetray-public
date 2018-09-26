# Basic steamshovel driver script for testing

# Because this script uses time.sleep(), it needs to be run with steamshovel --console;
# Using it with the embedded ipython interaction widget will just hang the GUI.

from icecube.shovelart import vec3d
from Tank import Tank
from time import sleep

timeline = window.timeline
scenario = window.gl.scenario

scenario.add( "Ice", [] )

# Look at the ice surface
window.gl.cameraLoc = vec3d(1000,1000,2100)
window.gl.cameraPivot = vec3d(0,0,1900)

# Put a panzer on the ice surface
scenario.registerArtist( Tank )
t = scenario.add( "Tank", [] )

# Play for five seconds
timeline.time = 0
timeline.eventEndTime = 10000
timeline.play()
sleep(5)

# Remove and re-add the tank, plus a new label
scenario.remove( t )
scenario.add( "TimeLabel", [] )
t = scenario.add( "Tank", [] )

# Five more seconds
timeline.rewind()
timeline.play()
sleep(5)

app.quit()
