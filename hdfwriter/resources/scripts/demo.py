#!/usr/bin/env python3
# 
# copyright  (C) 2010
# The Icecube Collaboration
# 
# $Id: demo.py 136437 2015-08-18 22:28:14Z hdembinski $
# 
# @version $Revision: 136437 $
# @date $LastChangedDate: 2015-08-18 18:28:14 -0400 (Tue, 18 Aug 2015) $
# @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy: hdembinski $
#
# Demonstrating how to book data from I3 files.

import os
from optparse import OptionParser

parser = OptionParser()
parser.add_option("-f","--format",dest="format",help="format to output [hdf5, root, or csv]",default='hdf5')
parser.add_option("-z","--compress",dest="compression",help="compression level",default=1,type=int)
parser.add_option("-n","--frames",dest="nframes",help="number of frames to process",default=None,type=int)

options,args = parser.parse_args()

if len(args) != 1:
	parser.error("You must supply an input filename")
	
infile = args[0]
outfile = os.path.basename(args[0]) + '.' + options.format

from icecube import icetray,dataclasses,dataio,tableio
from I3Tray import I3Tray

if options.format == 'hdf5':
	from icecube import hdfwriter
	tabler = hdfwriter.I3HDFTableService(outfile,options.compression)
elif options.format == 'root':
	from icecube import rootwriter
	tabler = rootwriter.I3ROOTTableService(outfile,options.compression)
elif options.format == 'csv':
	tabler = tableio.I3CSVTableService(outfile[:-4] + '_csv')
else:
	raise ValueError("I don't have a writer service for format '%s'"%options.format)

tray = I3Tray()

tray.AddModule('I3Reader','reader',filename=infile)

count = 0
def counter(frame):
    global count
    if (count%100==0):
        print("%d frames"%count)
    count +=1

# tray.AddModule(tableio.I3TableWriter,'writer1',
#     tableservice = tabler, keys=['LineFit', 'InIceRawData'], types=[dataclasses.I3Particle])


tray.AddModule(tableio.I3TableWriter,'writer1',
    tableservice = tabler, keys=['LineFit', 'InIceRawData'])

tray.AddModule(tableio.I3TableWriter,'writer2',
    tableservice = tabler, types=[dataclasses.I3Particle])



tray.AddModule(counter,'count-count')


if options.nframes is not None:
	tray.Execute(options.nframes)
else:
	tray.Execute()

