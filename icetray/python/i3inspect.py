# 
#  $Id: i3inspect.py 1793 2014-01-15 16:57:49Z jvansanten $
#  
#  Copyright (C) 2011
#  Jakob van Santen <vansanten@wisc.edu>
#  and the IceCube Collaboration <http://www.icecube.wisc.edu>
#  

from glob import glob
from icecube.icetray import I3Context, I3Configuration, I3Module, module_default_config
import types, sys, os, inspect, copy

try:
	import docutils.frontend
	import docutils.utils
	import docutils.parsers.rst
	from docutils import nodes        
	
	class ParamHarvester(nodes.SparseNodeVisitor):
		"""Extract Sphinxy parameter descriptions from a docstring."""
		def __init__(self, document):
			nodes.SparseNodeVisitor.__init__(self, document)
			self.tags = set(['param', 'parameter', 'arg', 'argument', 'key', 'keyword'])

		def visit_document(self, node):
			self.params = {}
			self.field_name = None
			self.field_body = None
			self.field_list = None

		def visit_field_list(self, node):
			self.field_list = node

		def depart_field(self, node):
			if self.field_list is not None and self.field_name is not None:
				self.field_list.remove(node)
			self.field_name = None
			self.field_body = None

		def visit_field_name(self, node):
			fields = node.children[0].split()
			if fields[0] in self.tags and len(fields) > 1:
				self.field_name = fields[1]
			else:
				self.field_name = None
			self.field_body = None

		def visit_field_body(self, node):

			self.field_body = node.astext()
			if len(self.field_body) == 0:
				self.field_body = None

		def depart_field_body(self, node):
			if self.field_name is not None and self.field_body is not None:
				self.params[str(self.field_name)] = self.field_body
	
	def getdoc(obj):
		docstring = inspect.getdoc(obj)
		if docstring is None:
			return ('', {})
			
		parser = docutils.parsers.rst.Parser()
		settings = docutils.frontend.OptionParser(
		    components=(docutils.parsers.rst.Parser,)).get_default_values()
		doc = docutils.utils.new_document('', settings)
		harvester = ParamHarvester(doc)
		
		parser.parse(docstring, doc)
		doc.walkabout(harvester)
		return (doc.astext().strip(), harvester.params)

except ImportError:
	
	def getdoc(obj):
		docstring = inspect.getdoc(obj)
		if docstring is None:
			return ('', {})
		else:
			return (docstring, {})

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
		doc, descdict = getdoc(segment)
		
		args = args[2:]
		defaults = defaults[2:]
		if hasattr(segment, 'additional_kwargs'):
			args += segment.additional_kwargs.keys()
			defaults += segment.additional_kwargs.values()
		descriptions = [descdict.get(k, '') for k in args]

		if hasattr(segment, 'module'): # Alt config, merge parent config
			rootconfig = get_configuration(segment.module)
			tweaked = dict(rootconfig)

			keycasemap = dict(zip([i.lower() for i in
			    rootconfig.keys()], rootconfig.keys()))
			if hasattr(segment, 'default_overrides'):
				for k in segment.default_overrides.keys():
					tweaked[keycasemap[k.lower()]] = \
					    segment.default_overrides[k]
			if hasattr(segment, 'remove_opts'):
				for k in segment.remove_opts:
					tweaked.pop(k)

			args += tweaked.keys()
			defaults += tweaked.values()
			descriptions += [rootconfig.descriptions[k] for k in
			    list(tweaked.keys())]

		config = cls(args, defaults, descriptions)
		config.__doc__ = doc
		return config

def get_configuration(module):
	"""Get an I3Module's default I3Configuration."""
	if hasattr(module, "__i3traysegment__"):
		return I3HoboConfiguration.from_traysegment(module)
	if isinstance(module, str):
		return module_default_config(module)
	elif isinstance(module, type) and issubclass(module, I3Module):
		return module(I3Context()).configuration
	else:
		raise TypeError("Module must be either a string naming a registered C++ subclass of I3Module or a Python subclass of I3Module, not %s" % type(module))

def is_I3Module(obj):
	return isinstance(obj, type) and issubclass(obj, I3Module)

def is_traysegment(obj):
	return hasattr(obj, "__i3traysegment__")

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

def harvest_objects(module,want):
	harvest = {}
	for item in dir(module):
		if item.startswith('_'):
			continue
		attr = getattr(module, item)
		if isinstance(attr, types.ModuleType) and attr.__name__.startswith(module.__name__):
			harvest.update(harvest_objects(attr,want))
		elif want(attr) and attr.__module__.startswith(module.__name__):
			harvest[attr]=item
	return harvest
	
def get_inspectable_projects():

	noinspect = [ os.path.basename(fname) for fname in
					 glob(os.path.join(os.environ['I3_BUILD'],
									   'docs','no_inspect','*'))]
	
	noinspect+=[ d.replace("-","_") for d in noinspect]
	noinspect=set(noinspect)

	libdir = os.path.join(os.environ['I3_BUILD'],'lib')
	
	cpp_libs = [os.path.basename(fname).split('.')[0][3:]
				for fname in glob(os.path.join(libdir,'lib*'))]

	cpp_libs = [ l for l in cpp_libs if l not in noinspect]

	python_libs = [os.path.basename(fname).split('.')[0]
				   for fname in glob(os.path.join(libdir,'icecube','*.so'))
				   if os.path.isfile(fname) ]

	python_libs = [ l for l in python_libs if l not in noinspect]

	python_dirs = [os.path.basename(fname)
				   for fname in glob(os.path.join(libdir,'icecube','*'))
				   if os.path.isdir(fname)]

	python_dirs = [ l for l in python_dirs if l not in noinspect]

	return cpp_libs,python_libs,python_dirs
