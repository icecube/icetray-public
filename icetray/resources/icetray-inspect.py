#!/usr/bin/env python
# 
#  $Id: icetray-inspect.py 2188 2015-07-30 04:09:28Z nega $
#  
#  Copyright (C) 2011
#  Jakob van Santen <vansanten@wisc.edu>
#  and the IceCube Collaboration <http://www.icecube.wisc.edu>
#
import os.path
from optparse import Option, OptionParser
import re
import xml.etree.ElementTree as ET


def check_regex(option, opt, value):
	try:
		return re.compile(value)
	except:
		raise OptionValueError("'%s' is not a regular expression" % value)
	
Option.TYPES = Option.TYPES + ("regex",)
Option.TYPE_CHECKER["regex"] = check_regex

parser = OptionParser("usage: %prog [options] project1 project2 ...")
parser.add_option('-a', '--all', dest='all',
	help='Examine all projects/libraries in DIRECTORY', metavar='DIRECTORY',
	default=None)
parser.add_option('-R', '--regex', dest='regex', type='regex',
	help='Print only modules/services/segments whose names match this regular expression',
	default=None)
parser.add_option('-s', '--sphinx', dest='sphinx', help='Output in XML',
	action='store_true', default=False)
parser.add_option('-x', '--xml', dest='xml', help='Output in XML',
	action='store_true', default=False)
parser.add_option('-o', '--output-file',dest='output',default=None,
	help='filename to write output')
parser.add_option('--verbose-docs', dest='verbose_docs', action='store_true',
	help='Print long form of docs strings',
	default=False)
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
	
from icecube import icetray, dataclasses,tableio
from icecube.icetray import i3inspect
from icecube.icetray import traysegment
import inspect, re, glob, sys, cgi
from os.path import splitext, basename

if opts.all:
	args += [splitext(basename(fname))[0][3:] for fname in glob.glob('%s/lib*' % opts.all)]

bad_services = ['I3TrayInfoServiceFactory'] # Services that crash on instantiation
bad_libs = ['corsikaXX', 'xppc', 'ppc'] # Libraries that segfault or rudely call exit()

modcount = 0
servicecount = 0
segmentcount = 0
convcount = 0

# keep our output clean.
icetray.I3Logger.global_logger = icetray.I3NullLogger()


class sphinx_writer:
	def __init__(self,file):
		self.file = file

	def file_header(self):
		self.file.write('IceTray Module Reference\n')
		self.file.write('========================\n\n')

	def file_footer(self):
		pass
		
	def project_header(self,project):
		self.file.write('Project %s\n' % project)
		self.file.write('-'*(len(project)+8)+'\n\n')

	def project_footer(self):
		pass

	def module_header(self,modname,category,docs):

		self.file.write("**%s** (%s)\n\n"
						%(modname,category))
		
		if docs:
			for d in docs.strip().splitlines():
				self.file.write('    %s\n' % d)
			self.file.write('\n')
			


	def module_footer(self):
		self.file.write('\n')

	def parameters(self,config):
		desc = config.descriptions
		for k in config.keys():
			try:
				default = repr(config[k])				
			except RuntimeError:
				default = "<Unprintable>"
				
			self.file.write('-  **%s**, Default = %s, %s\n'
							%(k,default,desc[k].replace('\n', '')))

	def segment_header(self):		
		pass

	def segment_footer(self):
		self.file.write('\n')
		pass
			

	def segelement(self,modtype,component_name,name,args):
		line = '    Add%s(%s, \'%s\'' % (modtype, component_name, name)
		for i in args.keys():
			line += ', %s=%s' % (i, args[i].__repr__())
		line += ')\n'
		self.file.write(line)



class xml_writer:
	def __init__(self,file):
		self.file = file

	def file_header(self):
		self.file.write('<?xml version=\'1.0\'?>\n')
		self.file.write('<icetray-inspect>\n')

	def file_footer(self):
		self.file.write('</icetray-inspect>\n')	
		
	def project_header(self,project):
		self.file.write('<project name="%s">\n' % project)

	def project_footer(self):
		self.file.write('</project>\n')

	def module_header(self,modname,category,docs):
		self.file.write('<module>\n')
		self.file.write('<type>%s</type>\n' % cgi.escape(modname))
		self.file.write('<kind>%s</kind>\n' % cgi.escape(category))
		self.file.write('<description>%s</description>\n' % cgi.escape(docs))

	def module_footer(self):
		self.file.write('</module>\n')


	def segment_header(self):
		self.file.write('<segment>\n')

	def segment_footer(self):
		self.file.write('</segment>\n')

	def parameters(self,config):
		desc = config.descriptions
		for k in config.keys():
			self.file.write('<parameter>\n')
			self.file.write('\t<name>%s</name>\n' % cgi.escape(k))
			self.file.write('\t<description>%s</description>\n'
							% cgi.escape(desc[k]))
			try:
				default = repr(config[k])				
			except RuntimeError:
				default = "[Unprintable]"
			self.file.write('\t<default_value>%s</default_value>\n'
							% cgi.escape(default))
			self.file.write('</parameter>\n')

	def segelement(self,modtype,component_name,name,args):
		self.file.write('\t<segelement type="%s" name="%s" instance="%s">\n'
						% (modtype, cgi.escape(component_name), cgi.escape(name)))
		for i in args.keys():
			self.file.write('\t<argument name="%s">%s</argument>\n'
				  % (cgi.escape(i), cgi.escape(args[i].__repr__())))
		self.file.write('\t</segelement>\n')

class human_writer:
	def __init__(self,file):
		self.file = file

	def file_header(self):
		self.file.write('**** IceTray Inspect ****\n\n')

	def file_footer(self):
		self.file.write(
			'\n%d module, %d service, and %d tray segment configurations reported.\n'
			% (modcount, servicecount, segmentcount))

	def project_header(self,project):
		self.file.write('*** %s ***\n' % project)		
		self.file.write('-'*79+'\n')
		
	def project_footer(self):
		pass

	def module_header(self,modname,category,docs):
		self.file.write('  %s (%s)\n' % (modname, category))
		if not opts.names_only and len(docs) > 0:
			self.file.write('\n')
			self.file.write('    ' + docs.replace('\n', '\n    '))
			self.file.write('\n\n')
	def module_footer(self):
		pass

	def segment_header(self):
		self.file.write('  Equivalent to:\n')

	def segment_footer(self):
		self.file.write('\n')

	def parameters(self,config):
		desc = config.descriptions
		if len(config.keys()) > 0:
			self.file.write('  Parameters:\n')
			sortedkeys = list(config.keys())
			sortedkeys.sort()
			for k in sortedkeys:
				self.file.write('    %s\n' % k)
				if  desc[k]:
					self.file.write('      Description : %s\n' % desc[k])
				try:
					default = repr(config[k])
				except RuntimeError:
					default = "[Unpritable]"
				self.file.write('      Default     : %s\n' % default)

				self.file.write('\n')
		else:
			self.file.write('    (No parameters)\n')
		self.file.write('  ' + '-'*77 + '\n')


	def segelement(self,modtype,component_name,name,args):
		line = '    Add%s(%s, \'%s\'' % (modtype, component_name, name)
		for i in args.keys():
			line += ', %s=%s' % (i, args[i].__repr__())
			line += ')'
		line+='\n'
		self.file.write(line)


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
		output.segelement(modtype,component_name,name,args)
			
	def AddModule(self, module, name, **kwargs):
		self.PrintArgs('Module', module, name, kwargs)
	def AddService(self, module, name, **kwargs):
		self.PrintArgs('Service', module, name, kwargs)
	def AddSegment(self, module, name, **kwargs):
		self.PrintArgs('Segment', module, name, kwargs)
		
def print_segment(segment):
	potemkin = PotemkinTray()
	output.segment_header()
	try:
		segment(potemkin, 'example')
	except:
		raise
		sys.stderr.write('Error instantiating segment \'%s\' with default arguments\n'%
						 segment)
	output.segment_footer()


def get_doxygen_docstring(modulename):
	xmlfile = os.path.join(os.environ["I3_BUILD"],"docs","doxygen",
						   project,"xml","class"+modulename+".xml")
	try:
		tree = ET.parse(xmlfile)
	except IOError:
		return ""
	
	root = tree.getroot()
	comp = root.find("compounddef")
	brief = comp.find('briefdescription')

	doc = ET.tostring(brief, encoding='utf8', method='text')

	if opts.verbose_docs:
		detail = comp.find('detaileddescription')
		doc+='\n\n'+ET.tostring(detail, encoding='utf8', method='text')

	return doc.strip()
	

def display_config(mod, category, modname=None):
	if modname is None:
		modname = mod
			
	if opts.regex and not opts.regex.match(modname):
		return False
	try:
		config = i3inspect.get_configuration(mod)
	except RuntimeError:
		e = sys.exc_info()[1]
		sys.stderr.write("Error constructing '%s': %s\n" % (mod, e))
		return False


	if isinstance(mod, str):
		docs = get_doxygen_docstring(modname)
	elif inspect.getdoc(config) != None and len(inspect.getdoc(config)) > 0:
		docs = inspect.getdoc(config)
	elif inspect.getdoc(mod) != None and len(inspect.getdoc(mod)) > 0:
		docs = inspect.getdoc(mod)
	else:
		docs = ''

	if not opts.verbose_docs:
		docs = docs.strip().split('\n')[0]
		
	output.module_header(modname,category,docs)
	
	if i3inspect.is_traysegment(mod) and opts.expand_segments:
		print_segment(mod)
		
	if not opts.names_only:
		output.parameters(config)
		
	output.module_footer()
	return True

def get_converters(project):
	converters = []
	for obj,convs in tableio.I3ConverterRegistry.registry.items():
		for conv in convs:
			if conv.__module__.split('.')[1]==project:
				converters.append(conv)
	return converters


if opts.output:
	outfile = open(opts.output,'wt')
else:
	outfile = sys.stdout

if opts.sphinx:
	output = sphinx_writer(outfile)
elif opts.xml:
	output = xml_writer(outfile)
else:
	output = human_writer(outfile)


output.file_header()
for project in args:
	if project in bad_libs:
		continue
	
	try:
		modname = project.replace('-','_')
		module = __import__('icecube.%s' % modname, globals(), locals(), [modname])
		py_modules = i3inspect.harvest_objects(module, i3inspect.is_I3Module)
		traysegments = i3inspect.harvest_objects(module, i3inspect.is_traysegment)
	except ImportError:
		try:
			icetray.load(project, False)
			py_modules = []
			traysegments = []
		except RuntimeError:
			e = sys.exc_info()[1]
			sys.stderr.write("Ignoring '%s': %s\n" % (project, e))
			continue
		
	cxx_modules = icetray.modules(project)
	cxx_services = icetray.services(project)

	#converters = get_converters(project)
		
	output.project_header(project)
	
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


	if False:
		for converter in converters:
			display_config(converter, 'TableIO Converter')
			convcount += 1

	output.project_footer()
			
output.file_footer()

