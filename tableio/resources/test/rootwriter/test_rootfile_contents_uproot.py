#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2025 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

###
# Script to make sure that the rootfile produced by this project has a proper MasterTree
###

import os, sys
import numpy

try:
    import uproot
except ModuleNotFoundError as e:
    print(f"Skipping test: {e.msg}")
    print("Is your installation correct?")
    sys.exit(0)

from icecube.icetray import I3Tray
from icecube.tableio import I3TableWriter
from icecube.tableio import I3ROOTTableService
from icecube import dataclasses
from icecube import dataio

### ----- RUN A SCRIPT -------
## This bit is similar to resources/examples/book_data.py.  Well, copied, basically...

TESTDATA = os.path.expandvars("$I3_TESTDATA")
rootfile = "mytestroot.uproot.root"
infiles = [TESTDATA+"/sim/Level2_IC86.2011_corsika.010281.001664.00.i3.bz2"]

tray = I3Tray()

# Read .i3 file
tray.AddModule("I3Reader", "reader",
               FileNameList = infiles)

# Open output file
table_service = I3ROOTTableService(filename= rootfile,  #Name of the output TFile which contains TTree.
                                   master= "FullTree", #Default name: "MasterTree".
                                   #mode=RECREATE,     #RECREATE is the default one, more options (NEW,UPDATE,etc) in:
                                                       #https://root.cern.ch/root/html/TFile.html .
                                   )

# Book data
tray.AddModule(I3TableWriter, "writer",
               TableService = table_service,
               SubEventStreams= ['in_ice'],           #Book events happening InIce. One also can use SubEventStreams= ['in_ice','ice_top'].
               Keys = [ "SPEFit2", "SPEFitSingle" ],  #This list has to contain all the objects that you want to book.
               #BookEverything=True,                  #Will book every thing in the frame, and your file will be very large,
                                                      #we suggest "do not do that". Default is False.
               )

tray.Execute()
del tray

### ----- NOW TEST THE OUTPUT -------
## Failures that have happened in the past... MasterTree properly "synced/aligned" with the other trees?

with uproot.open(rootfile) as f:
    tm = f["FullTree"]
    t1 = f["SPEFit2"]
    t2 = f["SPEFitSingle"]

    ## Test the number of entries, including that the Master Tree has all of 'em.
    assert(tm.num_entries == 2229)
    assert(t1.num_entries == 2229)
    assert(t2.num_entries == 2229)

    hx, _ = numpy.histogram(t1["x"], bins=20, range=(-1000, 1000))
    expectedcontents = [1, 1, 1, 2, 27, 97, 170, 227, 239, 277, 298, 257, 222, 186, 115, 43, 2, 0, 4, 0]
    assert((hx == expectedcontents).all())

if os.path.exists(rootfile):
    os.remove(rootfile)
