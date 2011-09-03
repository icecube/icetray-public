# 
#  $Id$
#  
#  Copyright (C) 2011
#  Jakob van Santen <vansanten@wisc.edu>
#  and the IceCube Collaboration <http://www.icecube.wisc.edu>
#  

from icecube.icetray import I3Context, I3Configuration, Connections, I3Module, _instantiate_module
import types, sys, os

def make_context():
	"""Create a potemkin I3Context suitable for instantiating an I3Module from Python."""
	context = I3Context()
	
	# Module configuration parameters go here.
	config = I3Configuration()
	context['I3Configuration'] = config
	
	# Fake outboxes.
	outboxes = Connections()
	context['OutBoxes'] = outboxes
	
	return context
	
def get_configuration(module):
	"""Get an I3Module's default I3Configuration."""
	context = make_context()
	context.configuration.ClassName = str(module)
	if isinstance(module, str):
		_instantiate_module(module, context)
	elif isinstance(module, type) and issubclass(module, I3Module):
		module(context)
	else:
		raise TypeError, "Module must be either a string naming a registered C++ subclass of I3Module or a Python subclass of I3Module, not %s" % type(module)
	
	return context.configuration

major, minor = sys.version_info[:2]
if major >= 2 and minor >= 5:
	def same_package(current, other):
		return current.__package__ == other.__package__
else:
	def hobo_getpackage(mod):
		"""
		A stupid hack for ancient Python. This does not work in general,
		but should be fine for modules installed as $I3_BUILD/lib/icecube/foo.
		"""
		# Exclude built-ins
		if not hasattr(mod, '__file__'):
			return None
		i3base = os.path.expandvars('$I3_BUILD/lib/icecube')
		package = mod.__file__[len(i3base)+1:].split(os.sep)[0]
		# Maybe we have a compiled extension
		base, ext = os.path.splitext(package)
		if ext == '.dylib' or ext == '.so':
			package = base
		return package
	def same_package(current, other):
		return hobo_getpackage(current) == hobo_getpackage(other)
	
def harvest_subclasses(module, klass=I3Module, memo=None):
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
	excluded = lambda klass_: not same_package(sys.modules[klass_.__module__], module)

	# Get public properties (underscored names are private by convention)
	harvest = []
	for item in dir(module):
		if item.startswith('_'):
			continue
		attr = getattr(module, item)
		if isinstance(attr, types.ModuleType) and same_package(attr, module):
			harvest += harvest_subclasses(attr, klass, memo)
		elif isinstance(attr, type):
			if issubclass(attr, klass) and not excluded(attr):
				harvest.append(attr)
	
	# Ensure uniqueness.
	return list(set(harvest))
	
