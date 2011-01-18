# -*- coding: utf-8 -*-
# 
# copyright  (C) 2011
# The Icecube Collaboration
# 
# $Id$
# 
# @version $Revision$
# @date $LastChangedDate$
# @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$
#

from icecube.icetray import I3ConditionalModule
import unittest

def I3TestModuleFactory(*test_cases):
	"""
Create an I3Module that runs the given unittest test cases on each Physics frame.

Usage:

class I3TimeHorizonCutTest(unittest.TestCase):
	def testKeys(self):
		\"""All keys are present.""\"
		self.assertTrue('InIceRawData' in self.frame, "Up is down, black is white!")
		self.assertTrue('InIceRawData_Horizon' in self.frame, "Up is down, black is white!")
		
	def testSizeReduction(self):
		\"""Output map is ever so slightly smaller""\"
		inlaunches = self.frame['InIceRawData']
		outlaunches = self.frame['InIceRawData_Horizon']
		self.assert_(len(outlaunches) <= len(inlaunches))
		
tray.AddModule(icetray.I3TestModuleFactory(I3TimeHorizonCutTest), 'test',
	If=lambda frame: 'InIceRawData' in frame and len(frame['InIceRawData']) > 0)

	"""
	class I3TestModule(I3ConditionalModule):
		runner = unittest.TextTestRunner()
		def __init__(self, context):
			I3ConditionalModule.__init__(self, context)
			self.AddOutBox("OutBox")
			
			self.suites = []
			for case in test_cases:
				self.suites.append(unittest.defaultTestLoader.loadTestsFromTestCase(case))
			self.test_cases = test_cases
			
		def Configure(self):
			pass
		
		def Physics(self, frame):
			result = None
			
			for case in self.test_cases:
				case.frame = frame
			for test in self.suites:
				if result is None:
					result = self.runner.run(test)
				else:
					result = self.runner.run(test, result)
			
			if not result.wasSuccessful():
				self.RequestSuspension()
			else:
				self.PushFrame(frame)
		
		def Finish(self):
			pass
	
	return I3TestModule
