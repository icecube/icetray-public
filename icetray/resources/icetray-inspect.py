#!/usr/bin/env python
# 
#  $Id$
#  
#  Copyright (C) 2011
#  Jakob van Santen <vansanten@wisc.edu>
#  and the IceCube Collaboration <http://www.icecube.wisc.edu>
#

from optparse import OptionParser

parser = OptionParser("usage: %prog [options] project1 project2 ...")
parser.add_option('-a', '--all', dest='all',
	help='Examine all projects/libraries in DIRECTORY', metavar='DIRECTORY',
	default=None)
parser.add_option('-x', '--xml', dest='xml', help='Output in XML',
	action='store_true', default=False)
parser.add_option('--no-params', dest='names_only', action='store_true',
	help='Print the names of modules only, not their parameters',
	default=False)
parser.add_option('--no-modules', dest='no_modules', action='store_true',
	help='Skip I3Modules',
	default=False)
parser.add_option('--no-services', dest='no_services', action='store_true',
	help='Skip ServiceFactories',
	default=False)
parser.add_option('--no-segments', dest='no_segments', action='store_true',
	help='Skip I3Tray segments',
	default=False)
parser.add_option('--expand-segments', dest='expand_segments', action='store_true',
	help='Expand I3Tray segments, printing the sequence of modules they add to the tray',
	default=False)

opts, args = parser.parse_args()
if len(args) == 0 and opts.all is None:
	parser.print_help()
	parser.exit(1)
	
from icecube import icetray, dataclasses
from icecube.icetray import i3inspect
from icecube.icetray import traysegment
import inspect, re, glob, sys, cgi
from os.path import splitext, basename

if sys.version_info[:2] < (2,6):
	from icecube.icetray.inspecthelpers2 import i3inspect_getconfig,i3inspect_load
else:
	from icecube.icetray.inspecthelpers3 import i3inspect_getconfig,i3inspect_load
	
if opts.all:
	args += [splitext(basename(fname))[0][3:] for fname in glob.glob('%s/lib*' % opts.all)]

bad_services = ['I3TrayInfoServiceFactory'] # Services that crash on instantiation
bad_libs = ['corsikaXX', 'xppc', 'ppc'] # Libraries that segfault or rudely call exit()

modcount = 0
servicecount = 0
segmentcount = 0

# keep our output clean.
icetray.I3Logger.global_logger = icetray.I3NullLogger()

def print_config(config):
	desc = config.descriptions
	if len(config.keys()) > 0:
		sortedkeys = list(config.keys())
		sortedkeys.sort()
		for k in sortedkeys:
			print('    %s' % k)
			print('      Description : %s' % desc[k])
			try:
				print('      Default     : %s' % repr(config[k]))
			except RuntimeError:
				# Sometimes unrepresentable
				pass
			print('')
	else:
		print('    (No parameters)')
	print('  ' + '-'*77)
	
	
def print_segment(segment):
	class PotemkinTray:
		def PrintArgs(self, modtype, component, name, args):
			# Try lots of ways to get the name of modules that
			# work even if they are Python modules. Mostly
			# valid.
			try:
				component_name = component.__repr__()
			except TypeError:
				try:
					component_name = component.__module__ + component.__name__
				except AttributeError:
					component_name = repr(component)

			# Provide a simulacram of what the call looked like
			if opts.xml:
				print('\t<segelement type="%s" name="%s" instance="%s">' % (modtype, cgi.escape(component_name), cgi.escape(name)))
				for i in args.keys():
					print('\t<argument name="%s">%s</argument>' % (cgi.escape(i), cgi.escape(args[i].__repr__())))
				print('\t</segelement>')
			else:
				line = '    Add%s(%s, \'%s\'' % (modtype, component_name, name)
				for i in args.keys():
					line += ', %s=%s' % (i, args[i].__repr__())
				line += ')'
				print(line)
			
		def AddModule(self, module, name, **kwargs):
			self.PrintArgs('Module', module, name, kwargs)
		def AddService(self, module, name, **kwargs):
			self.PrintArgs('Service', module, name, kwargs)
		def AddSegment(self, module, name, **kwargs):
			self.PrintArgs('Segment', module, name, kwargs)

	potemkin = PotemkinTray()
	if opts.xml:
		print('<segment>')
	else:
		print('  Equivalent to:')
	try:
		segment(potemkin, 'example')
	except:
		if not opts.xml:
			print('    Error instantiating segment with default arguments')
	if opts.xml:
		print('</segment>')
	else:
		print('')

def print_xmlconfig(config):
	desc = config.descriptions
	if len(config.keys()) > 0:
		for k in config.keys():
			print('<parameter>')
			print('\t<name>%s</name>' % cgi.escape(k))
			print('\t<description>%s</description>' % cgi.escape(desc[k]))
			try:
				print('\t<default_value>%s</default_value>' % cgi.escape(repr(config[k])))
			except RuntimeError:
				# Sometimes unrepresentable
				pass
			print('</parameter>')

def display_config(mod, category, modname=None):
		if modname is None:
			modname = mod
			
		try:
			config = i3inspect_getconfig(mod)
		except RuntimeError:
			return False
			
		if isinstance(mod, str):
			docs = ''
		elif inspect.getdoc(config) != None and len(inspect.getdoc(config)) > 0:
			docs = inspect.getdoc(config)
		elif inspect.getdoc(mod) != None and len(inspect.getdoc(mod)) > 0:
			docs = inspect.getdoc(mod)
		else:
			docs = ''

		if opts.xml:
			print('<module>')
			print('<type>%s</type>' % cgi.escape(modname))
			print('<kind>%s</kind>' % cgi.escape(category))
			print('<description>%s</description>' % cgi.escape(docs))
		else:
			print('  %s (%s)' % (modname, category))
			if not opts.names_only and len(docs) > 0:
				print('')
				print('    ' + docs.replace('\n', '\n    '))
				print('')
		if i3inspect.is_traysegment(mod) and opts.expand_segments:
			print_segment(mod)
			
		if not opts.names_only:
			if opts.xml:
				print_xmlconfig(config)
			else:
				if len(docs) > 0:
					print('  Parameters:')
				print_config(config)
				

		if opts.xml:
			print('</module>')
		return True
		
if opts.xml:
	print('<?xml version=\'1.0\'?>')
	print('<icetray-inspect>')

for project in args:
	if project in bad_libs:
		continue
	
	if opts.xml:
		print('<project name="%s">' % project)
	else:
		print('*** %s ***' % project)
	
	try:
		modname = project.replace('-','_')
		module = __import__('icecube.%s' % modname, globals(), locals(), [modname])
		py_modules = i3inspect.harvest_objects(module, i3inspect.is_I3Module)
		traysegments = i3inspect.harvest_objects(module, i3inspect.is_traysegment)
	except ImportError:
		if i3inspect_load(project):
			py_modules = []
			traysegments = []
		else:
			continue
		
	cxx_modules = icetray.modules(project)
	cxx_services = icetray.services(project)

	if not opts.xml:
		print('-'*79)

	if not opts.no_modules:
		for mod in cxx_modules:
			if display_config(mod, 'C++ I3Module'):
				modcount += 1 
		for mod in py_modules:
			if display_config(mod, 'Python I3Module', py_modules[mod]):
				modcount += 1
	if not opts.no_services:
		for mod in cxx_services:
			if mod in bad_services:
				continue
			if display_config(mod, 'C++ ServiceFactory'):
				servicecount += 1
				
	if not opts.no_segments:
		for segment in traysegments:
			display_config(segment, 'I3Tray segment', traysegments[segment])
			segmentcount += 1
			
	if opts.xml:
		print('</project>')
	
if opts.xml:
	print('</icetray-inspect>')
else:
	print('')
	print('%d module, %d service, and %d tray segment configurations reported.' % (modcount, servicecount, segmentcount))
