#!/usr/bin/env python
#
# std_map_indexing_suite_dict_interface_test.py
#
# A test demonstrating the dict methods that work
# with std_map_indexing_suite.
# JvS 2009-03-27

import unittest, sys

try:
	sorted([3,2,1])
except NameError:
	print('sorted() is not defined, providing a work-around for python < 2.3')
	import copy
	def sorted(lst):
		cpy = list(copy.copy(lst)) 
		cpy.sort()
		return cpy

class I3MapDictInterfaceTest(unittest.TestCase):
	"""Kick the tires on the dict interface.
	
	This case has tests defined for all methods of 
	dict in Python 2.5."""
	# Non-implemented methods:
	# comparison (all)
	# __hash__
	# __reduce__, __reduce_ex__ (for pickling support)
	# setdefault (would have to determine a default for every C++ type)
	# fromkeys(k,None) (ditto)
	# update (kwargs as keys can only work with string maps)
	def setUp(self):
		from icecube import icetray,dataclasses
		self.mapClass = dataclasses.TestMapStringString
		self.dict = {'foo':'bar', 'bom':'baz', 'steam':'locomotive'} 
		self.map = self.mapClass()
		for k in self.dict.keys(): self.map[k] = self.dict[k]
	
	def test___class__(self):
		"""Obviously not the same class. Skip."""
		pass
	def test___cmp__(self):
		"""Not implementing comparison for now. Skip."""
		pass
	def test___contains__(self):
		"""dict.__contains__() is equivalent to map.__contains__()"""
		for k in self.dict.keys():
			self.assertEquals(k in self.dict,k in self.map)
		nokey = 'not a key'
		self.assertEquals(nokey in self.dict,nokey in self.map)
		pass
	def test___delattr__(self):
		"""Inherited from <object>. Skip."""
		pass
	def test___delitem__(self):
		"""dict.__delitem__() is equivalent to map.__delitem__()"""
		k = list(self.dict.keys())[0]
		self.assertEquals(self.dict.__delitem__(k),self.map.__delitem__(k))
		self.assertEquals(k in self.map,k in self.dict)
		pass
	def test___doc__(self):
		"""Irrelevant. Skip."""
		pass
	def test___eq__(self):
		"""Not implementing comparison for now. Skip."""
		pass
	def test___ge__(self):
		"""What does </> even mean for a dict? Skip."""
		pass
	def test___getattribute__(self):
		"""Inherited from <object>. Skip."""
		pass
	def test___getitem__(self):
		"""dict.__getitem__() is equivalent to map.__getitem__()"""
		for k in self.dict.keys():
			self.assertEquals(self.dict.__getitem__(k),self.dict.__getitem__(k))
		pass
	def test___gt__(self):
		"""What does </> even mean for a dict? Skip."""
		pass
	def test___hash__(self):
		"""Not strictly speaking equal to each other. Skip."""
		pass
	def test___init__(self):
		"""map accepts the same constructors as dict"""
		newMap = self.mapClass()
		self.assertEquals(newMap.items(),[])
		newMap = self.mapClass(self.dict)
		self.assertEquals(sorted(list(newMap.items())),sorted(list(self.dict.items())))
		newMap = self.mapClass(list(self.dict.items()))
		self.assertEquals(sorted(newMap.items()),sorted(self.dict.items()))
		
		pass
	def test___iter__(self):
		"""dict.__iter__() is equivalent to map.__iter__()"""
		# dict iterates over keys, but the map returns
		# wrapped pairs (i.e. items)
		fromMap =  [iterate[0] for iterate in self.map]
		fromDict = [iterate for iterate in self.dict]
		self.assertEquals(sorted(fromMap),sorted(fromDict))
		pass
	def test___le__(self):
		"""What does </> even mean for a dict? Skip."""
		pass
	def test___len__(self):
		"""dict.__len__() is equivalent to map.__len__()"""
		self.assertEquals(self.dict.__len__(),self.map.__len__())
		pass
	def test___lt__(self):
		"""What does </> even mean for a dict? Skip."""
		pass
	def test___ne__(self):
		"""Not implementing comparison for now. Skip."""
		pass
	def test___new__(self):
		"""Inherited from <object>. Skip."""
		pass
	def test___reduce__(self):
		"""Don't support pickling for now. Skip."""
		pass
	def test___reduce_ex__(self):
		"""Don't support pickling for now. Skip."""
		pass
	def test___repr__(self):
		"""Don't care how it prints. Skip."""
		pass
	def test___setattr__(self):
		"""Inherited from <object> Skip."""
		pass
	def test___setitem__(self):
		"""dict.__setitem__() is equivalent to map.__setitem__()"""
		k = 'a new key!'; v = 'a new value!'
		self.assertEquals(k in self.dict,False)
		self.assertEquals(k in self.map,False)
		self.dict.__setitem__(k,v)
		self.map.__setitem__(k,v)
		self.assertEquals(self.map[k],self.dict[k])
		pass
	def test___str__(self):
		"""Don't care how it prints. Skip."""
		pass
	def test_clear(self):
		"""dict.clear() is equivalent to map.clear()"""
		self.assertEquals(self.dict.clear(),self.map.clear())
		self.assertEquals([k for k,v in self.dict],[])
		self.assertEquals([k for k,v in self.map],[])
		pass
	def test_copy(self):
		"""dict.copy() is equivalent to map.copy()"""
		newMap = self.map.copy()
		newDict = self.map.copy()
		self.assertEquals(self.map.items(),newMap.items())
		self.assertEquals(sorted(self.dict.items()),sorted(newDict.items()))
		# FIXME: there seems to be no good way to test whether the copy was shallow
		# confirm that the copy was shallow
		# for dict_key,copied_dict_key in zip(sorted(self.dict.keys()),sorted(newDict.keys())):
		# 	self.assertEquals(id(dict_key),id(copied_dict_key))
		# 	self.assertEquals(id(self.dict[dict_key]),id(newDict[copied_dict_key]))
		# for map_key,copied_map_key in zip(sorted(self.map.keys()),sorted(newMap.keys())):
		# 	self.assertEquals(id(map_key),id(copied_map_key))
		# 	self.assertEquals(id(self.map[map_key]),id(newMap[copied_map_key]))
	
		pass
	def test_fromkeys(self):
		"""dict.fromkeys() is equivalent to map.fromkeys()"""
		v = list(self.dict.values())[0]
		newDict = self.dict.fromkeys(self.dict.keys(),v)
		newMap = self.map.fromkeys(self.map.keys(),v)
		self.assertEquals(sorted(self.dict.keys()),sorted(newDict.keys()))
		self.assertEquals(sorted(self.map.keys()),sorted(newMap.keys()))
		for k in self.dict.keys():
			self.assertEquals(newDict[k],v)
			self.assertEquals(newMap[k],v)
		pass
	def test_get(self):
		"""dict.get() is equivalent to map.get()"""
		default = 42
		key = list(self.dict.keys())[0]
		nokey = 'i am not a key'
		self.assertEquals(self.map[key],self.map.get(key,default))
		self.assertEquals(default,self.map.get(nokey,default))
		self.assertEquals(None,self.map.get(nokey))
		
		
		pass
	if sys.version_info[0] < 3:
		def test_has_key(self):
			"""dict.has_key() is equivalent to map.has_key()"""
			key = list(self.dict.keys())[0]
			nokey = 'i am not a key'
			self.assertEquals(self.map.has_key(key),self.dict.has_key(key))
			self.assertEquals(self.map.has_key(nokey),self.dict.has_key(nokey))
			self.assertEquals(self.map.has_key(key),True)
			self.assertEquals(self.map.has_key(nokey),False)
			pass
	def test_items(self):
		"""dict.items() is equivalent to map.items()"""
		dictItems = sorted(self.dict.items())
		mapItems = sorted(self.map.items())
		self.assertEquals(dictItems,mapItems)
		pass
	if sys.version_info[0] < 3:
		def test_iteritems(self):
			"""dict.iteritems() is equivalent to map.iteritems()"""
			dictItems = sorted(list(self.dict.iteritems()))
			mapItems = sorted(list(self.dict.iteritems()))
			self.assertEquals(dictItems,mapItems)
			pass
		def test_iterkeys(self):
			"""dict.iterkeys() is equivalent to map.iterkeys()"""
			dictItems = sorted(list(self.dict.iterkeys()))
			mapItems = sorted(list(self.dict.iterkeys()))
			self.assertEquals(dictItems,mapItems)
			pass
		def test_itervalues(self):
			"""dict.itervalues() is equivalent to map.itervalues()"""
			dictItems = sorted(list(self.dict.itervalues()))
			mapItems = sorted(list(self.dict.itervalues()))
			self.assertEquals(dictItems,mapItems)
			pass
	def test_keys(self):
		"""dict.keys() is equivalent to map.keys()"""
		dictItems = sorted(self.dict.keys())
		mapItems = sorted(self.map.keys())
		self.assertEquals(dictItems,mapItems)
		pass
	def test_pop(self):
		"""dict.pop() is equivalent to map.pop()"""
		default = 42
		key = list(self.dict.keys())[0]
		value = self.dict[key]
		nokey = 'i am not a key'
		self.assertEquals(self.map.pop(key,default),value)
		self.assertEquals(self.map.pop(key,default),default)
		self.assertRaises(KeyError,self.map.pop,key)
		self.assertRaises(KeyError,self.map.__getitem__,key)
		
		pass
	def test_popitem(self):
		"""dict.popitem() is equivalent to map.popitem()"""
		items = [self.map.popitem() for i in range(len(self.map))]
		self.assertEquals(sorted(items),sorted(self.dict.items()))
		self.assertEquals(len(self.map),0)
		self.assertRaises(KeyError,self.map.popitem)
		pass
	def test_setdefault(self):
		"""The template would have to know a default value for each type. Skip."""
		pass
	def test_update(self):
		"""dict.update() is equivalent to map.update()"""
		# only half-support update(self, dict, **kwargs):
		# can't add kwargs to map because of typing issues
		modpair = (list(self.dict.keys())[0],'a new value!')
		newpair = ('nogat','stuebl')
		oldkeys = list(self.dict.keys())[1:]
		newdict = {modpair[0]:modpair[1], newpair[0]:newpair[1]}
		self.map.update(newdict)
		for key in oldkeys:
			self.assertEquals(self.map[key],self.dict[key])
		for key in (modpair[0],newpair[0]):
			self.assertEquals(self.map[key],newdict[key])
		pass
	def test_values(self):
		"""dict.values() is equivalent to map.values()"""
		self.assertEquals(sorted(self.map.values()),sorted(self.dict.values()))
		pass


if __name__ == '__main__':
	unittest.main()
