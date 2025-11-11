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
    import ROOT
except ModuleNotFoundError as e:
    print(f"Skipping test... ROOT found, but its Python module is not importable: {e.msg}")
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
rootfile = "mytestroot.pyroot.root"
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

f = ROOT.TFile(rootfile)
tm = f.Get("FullTree")
t1 = f.Get("SPEFit2")
t2 = f.Get("SPEFitSingle")

## Test the number of entries, including that the Master Tree has all of 'em.
nm = tm.GetEntries()
n1 = t1.GetEntries()
n2 = t2.GetEntries()
#print("Nentries: ", nm, n1, n2)
assert(nm == 2229)
assert(n1 == 2229)
assert(n2 == 2229)

## Test that Draw/Fill will work:
hx = ROOT.TH1D("hx", "histogram of x", 20, -1000, 1000)
tm.Draw("SPEFit2.x>>hx")
expectedcontents = [1, 1, 1, 2, 27, 97, 170, 227, 239, 277, 298, 257, 222, 186, 115, 43, 2, 0, 4, 0]
for i in range(1,21):
  #print("Contents: ", hx.GetBinContent(i))
  assert(hx.GetBinContent(i) == expectedcontents[i-1])

## Test that SetBranchAddress() will work from the Master Tree
#x = numpy.array( 'i', [0] )
#x = ROOT.Double_t(0.0)
x = numpy.zeros(10)
tm.SetBranchAddress("SPEFit2.x", x)
firsttenentries = [-447.513139, -383.219767, 92.119155, 168.234237, 393.430899, 281.576931, -278.744509, -386.299233, 33.547140, -306.301783]
for i in range(0,10):
  tm.GetEntry(i)
  #print("This Entry: ", x[0])
  assert(abs(x[0] - firsttenentries[i]) < 0.00001)

f.Close()

## Remove the test file created herein
if os.path.exists(rootfile):
    os.remove(rootfile)
