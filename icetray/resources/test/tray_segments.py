#!/usr/bin/env python
#
# Check that tray segments work
#

from I3Tray import *

from icecube import icetray

tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource","bottomless")

addnullsaltconfig = icetray.module_altconfig("AddNulls", Where=["somestuff"])

@icetray.traysegment
def checkanddump(tray, name, checkfor=["thisdoesntexist"]):
	# verify they are there
	tray.AddModule("FrameCheck", name + "_checker",
	    ensure_physics_has=checkfor)
	tray.AddModule("Dump", name + "_dump")

# Add the default alt-default config
tray.AddSegment(addnullsaltconfig, "addnulls")
# Add the alt-default config, but with one overridden
# Note: capitalization is deliberately wrong to test deduplication!
tray.AddSegment(addnullsaltconfig, "addnullsdiff", 
    where=["here", "there", "everywhere", "tonsastuff"])
tray.AddSegment(checkanddump, "nullcheck", # Deliberate miscapitalization below
    CheckFor=["somestuff", "here", "there", "everywhere", "tonsastuff"])
tray.AddModule("TrashCan","adios")
tray.Execute(10)
tray.Finish()

