#!/usr/bin/env python
#

from icecube import icetray 
import sys

class TestModule(icetray.I3Module):
	def __init__(self, ctx):
		icetray.I3Module.__init__(self, ctx)
		self.recursion_count = 0
	def Finish(self):
		self.recursion_count += 1
		if self.recursion_count > 1:
			print('Finish looping forever!')
			sys.exit(1)
		icetray.I3Module.Finish(self)

ctx = icetray.I3Context()
mod = TestModule(ctx)
mod.Finish()
