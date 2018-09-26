# -*- coding: utf-8 -*-
# 
# copyright  (C) 2010
# The Icecube Collaboration
# 
# $Id: enum3.py 107762 2013-07-01 22:55:31Z nwhitehorn $
# 
# @version $Revision: 107762 $
# @date $LastChangedDate: 2013-07-01 18:55:31 -0400 (Mon, 01 Jul 2013) $
# @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy: nwhitehorn $
# 

class baseEnum(int):
	name = None
	values = {}
	def __repr__(self):
		return self.name

class metaEnum(type):
	"""Helper metaclass to return the class variables as a dictionary "
	"""

	def __new__(cls, classname, bases, classdict):
		""" Return a new class with a "values" attribute filled
		"""

		newdict = {"values":{}}

		for k in classdict.keys():
			if not (k.startswith('_') or k == 'name' or k == 'values'):
				val = classdict[k]
				member = baseEnum(val)
				member.name = k
				newdict['values'][val] = member
				newdict[k] = member
		
		# Tell each member about the values in the enum
		for k in newdict['values'].keys():
			newdict['values'][k].values = newdict['values']
		# Return a new class with the "values" attribute filled
		return type.__new__(cls, classname, bases, newdict)

class enum(baseEnum, metaclass=metaEnum):
	"""This class mimicks the interface of boost-python-wrapped enums.
	
Inherit from this class to construct enumerated types that can
be passed to the I3Datatype, e.g.:

	class DummyEnummy(tableio.enum):
		Foo = 0
		Bar = 1
		Baz = 2

	desc = tableio.I3TableRowDescription()
	desc.add_field('dummy', tableio.I3Datatype(DummyEnummy), '', '')
"""

