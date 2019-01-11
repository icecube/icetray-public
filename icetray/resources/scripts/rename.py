#!/usr/bin/env python
#
# Checking that the driving time exists
#
import sys
from I3Tray import *

# load("libicetray")

tray = I3Tray()

# generate empty frames
tray.AddModule("BottomlessSource","bottomless")

tray.AddModule("AddNulls", "nulls")(
    ("where", ["here", "there", "everywhere", "tonsastuff"])
    )

# verify they are there
tray.AddModule("FrameCheck","checker")(
    ("ensure_physics_has", ["here", "there", "everywhere", "tonsastuff"])
    )

tray.AddModule("Dump","dump0")

tray.AddModule("Rename", "rename")(
    ("Keys", ["here", "here2", "there", "there2"])
    )

tray.AddModule("FrameCheck","checker2")(
    ("ensure_physics_has", ["here2", "there2", "everywhere", "tonsastuff"]),
    ("ensure_physics_hasnt", ["here", "there"])
    )
# print em.   lots of nulls, dump shouldn't crash.
tray.AddModule("Dump","dump")

# throw em out
tray.AddModule("TrashCan","adios")

tray.Execute(215)

print "gonna get usage"

u = tray.Usage()

print "Got Usage:", u

print "*****\n"*4
for entry in u:
    print "%25s => %s" % (entry.key(), entry.data())
print "*****\n"*4

print "Total system time=", sum([entry.data().systime for entry in u])
print "Total user time=  ",   sum([entry.data().usertime for entry in u])
print "Max ncall=        ",   max([entry.data().ncall for entry in u])

tray.Finish()

