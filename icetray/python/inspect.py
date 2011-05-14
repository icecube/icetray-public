# 
#  $Id$
#  
#  Copyright (C) 2011
#  Jakob van Santen <vansanten@wisc.edu>
#  and the IceCube Collaboration <http://www.icecube.wisc.edu>
#  

import icecube.icetray as icetray
import types, sys

def make_context():
	"""Create a potemkin I3Context suitable for instantiating an I3Module from Python."""
	context = icetray.I3Context()
	
	# Module configuration parameters go here.
	config = icetray.I3Configuration()
	context['I3Configuration'] = config
	
	# Fake outboxes.
	outboxes = icetray.Connections()
	context['OutBoxes'] = outboxes
	
	return context
	
def get_configuration(module):
	"""Get an I3Module's default I3Configuration."""
	context = make_context()
	context.configuration.ClassName = str(module)
	
	if isinstance(module, str):
		icetray._instantiate_module(module, context)
	elif issubclass(module, icetray.I3Module):
		module(context)
	else:
		raise TypeError, "Module must be either a string naming a registered C++ subclass of I3Module or a Python subclass of I3Module, not %s" % type(module)
	
	return context.configuration
	
def harvest_subclasses(module, klass=icetray.I3Module, memo=None):
	"""Recursively search through an object for subclasses of I3Module."""
	# I miss Ruby's ObjectSpace. Sniff.
	
	if memo is None:
		memo = {}
	
	# Avoid circular references like the plague
	ptr = id(module)
	if ptr in memo:
		return []
	# Mark this module as visited
	memo[ptr] = module
	
	# Filter out classes defined in packages other than this one.
	excluded = lambda klass_: sys.modules[klass.__module__].__package__ != module.__package__

	# Get public properties (underscored names are private by convention)
	harvest = []
	for item in dir(module):
		if item.startswith('_'):
			continue
		attr = getattr(module, item)
		if isinstance(attr, type) and issubclass(attr, klass) and not excluded(klass):
			harvest.append(attr)
		elif isinstance(attr, types.ModuleType) and attr.__package__ == module.__package__:
			harvest += harvest_subclasses(attr, klass, memo)
	
	# Ensure uniqueness.
	return list(set(harvest))
	