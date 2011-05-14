#!/usr/bin/env python
# 
#  $Id$
#  
#  Copyright (C) 2011
#  Jakob van Santen <vansanten@wisc.edu>
#  and the IceCube Collaboration <http://www.icecube.wisc.edu>
#

from optparse import OptionParser
from icecube import icetray
from icecube.icetray import inspect
import re, glob, sys
from os.path import splitext, basename

parser = OptionParser("usage: %prog [options] project1 project2 ...")
parser.add_option('-a', '--all', dest='all',
	help='Examine all projects/libraries in DIRECTORY', metavar='DIRECTORY',
	default=None)
parser.add_option('--names-only', dest='names_only', action='store_true',
	help='Print the names of modules only, not their parameters',
	default=False)

opts, args = parser.parse_args()
if len(args) == 0 and opts.all is None:
	parser.print_help()
	parser.exit(1)

if opts.all:
	args += [splitext(basename(fname))[0][3:] for fname in glob.glob('%s/lib*' % opts.all)]

modcount = 0

# keep our output clean.
icetray.disable_logging() 

for project in args:
	
	print '*** %s ***' % project
	
	try:
		module = __import__('icecube.%s' % project.replace('-','_'))
		py_modules = icetray.inspect.harvest_subclasses(module)
	except ImportError:
		try:
			icetray.load(project)
			py_modules = []
		except RuntimeError as e:
			sys.stderr.write("Ignoring '%s': %s" % (project, e))
			continue
		
	cxx_modules = icetray.modules(project)

	print '-'*79
	
	for mod in cxx_modules + py_modules:
		
		print '  %s' % mod
		
		try:
			config = inspect.get_configuration(mod)
			desc = config.descriptions
		except RuntimeError as e:
			sys.stderr.write("Error constructing '%s': %s" % (mod, e))
			continue
			
		modcount += 1
			
		if not opts.names_only:
			if len(config.keys()) > 0:
				for k in config.keys():
					print '    %s' % k
					print '      Description : %s' % config[k]
					print '      Default     : %s' % desc[k]
					print ''
			else:
				print '    (No parameters)'
			print '  ' + '-'*77
print ''
print '%d module configurations reported.' % modcount
