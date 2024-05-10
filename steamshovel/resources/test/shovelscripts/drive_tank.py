# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

# Basic steamshovel driver script for testing

# Because this script uses time.sleep(), it needs to be run with:
#   steamshovel -D --testmode --noconsole -s ./steamshovel/resources/test/shovelscripts/drive_tank.py
#
# Using it with the embedded ipython interaction widget will just hang the GUI.
#
# This doesn't do anything that other tests do, so don't add it to the list of tests in CMakeLists.txt

# The window and app objects are provided by steamshovel so:
#   ruff: noqa: F821


from time import sleep

from icecube.shovelart import vec3d

try:
    from Tank import Tank
except Exception as e:
    import os
    import sys
    sys.path.insert(0, os.environ['I3_SRC']+"/steamshovel/resources/test/shovelscripts")
    from Tank import Tank

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
