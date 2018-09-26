#!/usr/bin/env python 

from optparse import OptionParser
parser = OptionParser()
parser.add_option('-i','--infile',
                  dest='INFILE',
                  help='I3File to read.')
(options,args) = parser.parse_args()

from I3Tray import I3Units
from os.path import expandvars
        
from icecube import icetray 
from icecube import dataclasses 
from icecube import phys_services 
from icecube import dataio 

tray = I3Tray()

tray.AddModule("I3Reader","reader",FileName = options.INFILE)

tray.AddModule("I3EventCounter",
    EventHeaderName = "I3EventHeader",
    CounterStep = 100)

tray.Execute()

