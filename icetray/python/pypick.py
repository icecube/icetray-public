import copy

class pypick(object):
	"""
	Construct lazily-evaluated boolean expressions from callables with
	identical signatures.
	
	pypicks can be combined using the bitwise operators \&, \|, \~, and \^.
	The result e.g. of pypick(a) \| pypick(b) is another pypick that, when
	calles as pick(foo) returns (a(foo) or b(foo)).
	
	Examples
	--------- 
	>>> p = pypick(lambda t: True)
	>>> q = pypick(lambda t: False)
	>>> (p&q)(None)
	False
	>>> (p|q)(None)
	True
	>>> (~p|q)(None)
	False
	>>> (p^q)(None)
	True
	>>> (p^~q)(None)
	False
	"""
	
	def __init__(self, function):
		self._check_callable(function, int)
		self._callable = function
		self._invert = False
		# extra user-friendliness
		if hasattr(function, "__doc__") and function.__doc__ is not None:
			self.__doc__ = copy.copy(function.__doc__)
		self.__name__ = function.__name__
	
	def __call__(self, *args, **kwargs):
		v = self._callable(*args, **kwargs)
		if self._invert:
			return (not v)
		else:
			return v
		
	def _check_callable(self, other, do):
		if hasattr(other, "__call__"):
			return do()
		else:
			raise TypeError("PyPick must be constructed with a callable")
			
	def __and__(self, other):
		return self._check_callable(other, lambda : pypick(lambda *args, **kwargs: self(*args, **kwargs) and other(*args, **kwargs)))
	def __or__(self, other):
		return self._check_callable(other, lambda : pypick(lambda *args, **kwargs: self(*args, **kwargs) or other(*args, **kwargs)))
	def __xor__(self, other):
		return self._check_callable(other, lambda : pypick(lambda *args, **kwargs: bool(self(*args, **kwargs)) ^ bool(other(*args, **kwargs))))
	def __invert__(self):
		inv = copy.copy(self)
		inv._invert = (not inv._invert)
		return inv
