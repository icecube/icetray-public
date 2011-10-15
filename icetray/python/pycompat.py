"""
Provide Python built-in functions introduced after version 2.4.
"""

try:
	all	
except NameError:
	def all(iterable):
		"""
		all(iterable) -> bool

		Return True if bool(x) is True for all values x in the iterable.
		"""
		for element in iterable:
			if not element:
				return False 
		return True 

try:
	any	
except NameError:
	def any(iterable):
		"""
		any(iterable) -> bool

		Return True if bool(x) is True for any value x in the iterable.
		"""
		for element in iterable:
			if element:
				return True
		return False
