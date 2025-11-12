#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

# SPDX-FileCopyrightText: 2025 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import sys

from icecube.icetray import I3Tray

from icecube.tableio import I3TableWriter
from icecube.tableio import I3ROOTTableService
from icecube import dataclasses

from icecube import dataio 

outfile = sys.argv[1]
infiles = sys.argv[2:]

#EXAMPLE HOW TO RUN IT: python $I3_SRC/tableio/resources/examples/rootwriter_book_data.py myoutput.root /data/sim/IceCube/2011/filtered/level2/CORSIKA-in-ice/10309/00000-00999/Level2_IC86.2011_corsika.010309.000000.i3.bz2

tray = I3Tray()

# Read .i3 file
tray.AddModule("I3Reader", "reader",
               FileNameList = infiles)

# Open output file
table_service = I3ROOTTableService(filename= outfile,  #Name of the output TFile which contains TTree.
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


