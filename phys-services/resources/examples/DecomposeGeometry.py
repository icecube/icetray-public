#!/usr/bin/env python
from optparse import OptionParser
parser = OptionParser()
parser.add_option('-i','--infile',
                  dest='INFILE',
                  help='I3File to read.')
parser.add_option('-o','--outfile',
                  dest='OUTFILE',
                  help='I3File to write.')
(options,args) = parser.parse_args()

import sys
from I3Tray import I3Tray
from icecube import icetray, dataio, phys_services

if len(sys.argv) != 3: 
    print("usage: DecomposeGeometry.py input_file output_file")
    sys.exit(-1)

tray = I3Tray()
tray.AddModule('I3Reader', Filename = options.INFILE)
tray.AddModule('I3GeometryDecomposer', DeleteI3Geometry=False)
tray.AddModule('I3Writer', Filename = options.OUTFILE)
tray.Execute()

