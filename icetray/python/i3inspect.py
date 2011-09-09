# 
#  $Id$
#  
#  Copyright (C) 2011
#  Jakob van Santen <vansanten@wisc.edu>
#  and the IceCube Collaboration <http://www.icecube.wisc.edu>
#  

from icecube.icetray import I3Context, I3Configuration, Connections, I3Module, _instantiate_module
import types, sys, os, inspect

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

class I3HoboConfiguration(dict):
	"""Fake I3Configuration's interface for icetray-inspect."""
	def __init__(self, args, defaults, descriptions):
		self.descriptions = {}
		self.args = list(args)
		for arg, default, desc in zip(args, defaults, descriptions):
			self[arg] = default
			self.descriptions[arg] = desc

	def keys(self):
		return list(self.args)

	@classmethod
	def from_traysegment(cls, segment):
		args, varargs, kwargs, defaultvals = inspect.getargspec(segment)
		defaults = [None]*len(args)
		if defaultvals is not None:
			defaults[-len(defaultvals):] = defaultvals
		descriptions = ['']*len(args)
		config = cls(args[2:], defaults[2:], descriptions[2:])
		config.__doc__ = segment.__doc__
		return config

def get_configuration(module):
	"""Get an I3Module's default I3Configuration."""
	if hasattr(module, "__i3traysegment__"):
		return I3HoboConfiguration.from_traysegment(module)
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
if major >= 2 and minor >= 6:
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

def is_I3Module(obj):
	return isinstance(obj, type) and issubclass(obj, I3Module)

def is_traysegment(obj):
	return hasattr(obj, "__i3traysegment__")
	
def harvest_objects(module, want=is_I3Module, memo=None):
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
			harvest += harvest_objects(attr, want, memo)
		elif want(attr) and not excluded(attr):
			print attr
			harvest.append(attr)
	
	# Ensure uniqueness.
	return list(set(harvest))
	
