# 
#  $Id$
#  
#  Copyright (C) 2011
#  Nathan Whitehorn <nwhitehorn@icecube.wisc.edu>
#  and the IceCube Collaboration <http://www.icecube.wisc.edu>
#


def traysegment(function):
	"""Register a tray segment configuration function with icetray.
	Usage:
	@icetray.traysegment
	def altconfig(tray, name, arg=stuff):
	"""
	return function

