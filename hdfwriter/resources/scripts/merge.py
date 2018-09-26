#!/usr/bin/env python
# 
# copyright  (C) 2010
# The Icecube Collaboration
# 
# $Id: merge.py 136825 2015-08-25 07:39:27Z jvansanten $
# 
# @version $Revision: 136825 $
# @date $LastChangedDate: 2015-08-25 03:39:27 -0400 (Tue, 25 Aug 2015) $
# @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy: jvansanten $
# 

# concatenate tables, keeping track of indexes and such automatically

from optparse import OptionParser
import os

parser = OptionParser(usage='%prog [OPTIONS] FILE1 [FILE2, FILE3, ...]',
    description='Concatenate tables, optionally converting between supported table formats.')
parser.add_option("-f","--format",dest="format",help="format to output",default='hdf5', choices=('hdf5', 'root', 'csv'))
parser.add_option("-z","--compress",dest="compress",help="compression level",default=1,type=int)
parser.add_option("-n","--frames",dest="nframes",help="number of frames to process",default=None,type=int)
parser.add_option("-o","--output",dest="outfile",help="name of the output file",default=None,type=str)

options,args = parser.parse_args()
if len(args) < 1:
    parser.error("You must supply at least one input file")
    
infiles = args
iformat = 'hdf5' # only service that supports reading at this point
oformat = options.format
if options.outfile is None:
    outfile = os.path.basename(infiles[0]) + '.' + options.format
else:
    outfile = options.outfile

from icecube import icetray,tableio
from icecube.tableio import I3TableTranscriber

# try to import the appropriate services
if 'hdf5' in [iformat,oformat]:
	from icecube.hdfwriter import I3HDFTableService
if 'root' in [iformat,oformat]:
	from icecube.rootwriter import I3ROOTTableService
if 'csv' in [iformat,oformat]:
	from icecube.textwriter import I3CSVTableService

if iformat == 'hdf5':
	inservices = [(I3HDFTableService,(infile,1,'r')) for infile in infiles]
elif iformat == 'root':
	inservices = [(I3ROOTTableService,(infile,'r')) for infile in infiles]
else:
	raise "Unknown input format '%s'" % iformat
	
if oformat == 'hdf5':
	outservice = I3HDFTableService(outfile,options.compress,'w')
elif oformat == 'root':
	outservice = I3ROOTTableService(outfile,compression_level=options.compress)
elif oformat == 'csv':
	outservice = I3CSVTableService(outfile)
else:
	raise "Unknown out format '%s'" % oformat

for ctor,args in inservices:
    print('Merging %s'%args[0])
    inservice = ctor(*args)
    scribe = I3TableTranscriber(inservice,outservice)
    if options.nframes is not None:
        scribe.Execute(options.nframes)
    else:
        scribe.Execute()
    inservice.Finish()

outservice.Finish()
