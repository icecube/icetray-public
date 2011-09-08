# 
#  $Id$
#  
#  Copyright (C) 2011
#  Nathan Whitehorn <nwhitehorn@icecube.wisc.edu>
#  and the IceCube Collaboration <http://www.icecube.wisc.edu>
#

import inspect # the real inspect

def traysegment(function, name=None):
	"""Register a tray segment configuration function with icetray. The
	segment can then be added to a tray using I3Tray.AddSegment().

	Usage:
	@icetray.traysegment
	def segment(tray, name, arg=stuff):
	"""

	if inspect.getdoc(function) is None:
		raise ValueError, "I3Tray segments must have docstrings!"

	if name is None:
		module = inspect.getmodule(function)
		name = "%s.%s" % (module.__name__, function.__name__)
	
	function.__i3name__ = name
	
	project = name.split('.')[1]
	if not project in traysegment.segments:
		traysegment.segments[project] = []
	traysegment.segments[project].append(function)

	return function
	
traysegment.segments = dict()
	
_modules_by_file = {}
def find_module(path):
	"""
	Given a Python source file, walk through sys.modules to find the
	fully-qualified module it belongs to.
	"""
	import sys, os, types
	path = os.path.abspath(path)
	if path in _modules_by_file:
		return _modules_by_file[path]
	module = None
	for name, mod in sys.modules.iteritems():
		if not hasattr(mod, "__file__"):
			continue
		# allow .py to match .pyc and .pyo
		if mod.__file__[:len(path)] == path:
			_modules_by_file[path] = mod
			module = mod
			break
	return module
	
def stored_name(frame):
	"""
	Evil mucking about in CPython internals: given a stack frame at a function
	call point, find the name to which the result will be assigned.
	"""
	from opcode import opmap
	opcode = lambda name: chr(opmap[name])

	code = frame.f_code.co_code
	# Advance to the next opcode.
	i = frame.f_lasti+1
	while i < len(code) and code[i] != opcode('STORE_NAME'):
		i += 1
	if i == len(code):
		return None
	# Arguments to regular Python opcodes are 2 bytes.
	oparg = ord(code[i+1]) + ord(code[i+2])*256
	return frame.f_code.co_names[oparg]

def module_altconfig(module, **altdefargs):
	"""Register an alternate [sub]set of defaults for a module.
	Any parameters not specified in module_altconfig will use the
	original module defaults.

	Once added to a module's pybindings, an instance of the module with
	the specified alternate defaults can then be added to a tray using
	I3Tray.AddConfig() or I3Tray.AddSegment().

	Usage: altconfig1 = icetray.module_altconfig('I3ExampleModule', Par1=5)
	"""
	
	# Find out which module we were called from, and the name to which
	# the return value will be assigned.
	lastframe = inspect.currentframe().f_back
	assigned_name = stored_name(lastframe)
	module_name = find_module(lastframe.f_code.co_filename).__name__
	name = "%s.%s" % (module_name, assigned_name)
	
	def segment(tray, name, **args):
		mergedargs = altdefargs.copy()
		for userarg in args.keys():
			mergedargs[userarg] = args[userarg]

		# Arguments to AddModule are, in general, case insensitive
		# Resolve any duplicate arguments with case ambiguities in favor
		# of the user's values.
		lowercasekeys = [i.lower() for i in mergedargs.keys()]
		for arg in altdefargs.keys():
			if lowercasekeys.count(arg.lower()) > 1:
				# Deduplicate by removing the default key
				mergedargs.pop(arg)
			
		tray.AddModule(module, name, **mergedargs)

	# Mark what we're doing in various interesting places (like the docs)
	segment.__doc__ = "Alternate configuration for %s\n\nOverridden defaults:\n" % module
	for arg in altdefargs.keys():
		segment.__doc__ += "\t%s=%s\n" % (arg, altdefargs[arg])
	segment.module = module
	segment.default_overrides = altdefargs

	return traysegment(segment, name)

