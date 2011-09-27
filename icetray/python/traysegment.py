# 
#  $Id$
#  
#  Copyright (C) 2011
#  Nathan Whitehorn <nwhitehorn@icecube.wisc.edu>
#  and the IceCube Collaboration <http://www.icecube.wisc.edu>
#

import inspect # the real inspect

def traysegment(function):
	"""Register a tray segment configuration function with icetray. The
	segment can then be added to a tray using I3Tray.AddSegment().

	Usage:
	@icetray.traysegment
	def segment(tray, name, arg=stuff):
	"""

	if inspect.getdoc(function) is None:
		function.__doc__ = "I3Tray segments should have docstrings. This one doesn't. Fix it."

	if len(inspect.getargspec(function)[0]) < 2:
		raise ValueError, "I3Tray segments must have at least two arguments (tray, name)"

	function.__i3traysegment__ = True

	return function

def module_altconfig(module, **altdefargs):
	"""Register an alternate [sub]set of defaults for a module.
	Any parameters not specified in module_altconfig will use the
	original module defaults.

	Once added to a module's pybindings, an instance of the module with
	the specified alternate defaults can then be added to a tray using
	I3Tray.AddConfig() or I3Tray.AddSegment().

	Usage: altconfig1 = icetray.module_altconfig('I3ExampleModule', Par1=5)
	"""

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

	func = traysegment(segment)

	return func

