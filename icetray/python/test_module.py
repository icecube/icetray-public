# -*- coding: utf-8 -*-
# 
# copyright  (C) 2011
# The Icecube Collaboration
# 
# $Id: test_module.py 1635 2013-06-28 21:33:24Z nwhitehorn $
# 
# @version $Revision: 1635 $
# @date $LastChangedDate: 2013-06-28 17:33:24 -0400 (Fri, 28 Jun 2013) $
# @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy: nwhitehorn $
#

from icecube.icetray import I3ConditionalModule, I3Frame
import unittest, sys

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
			self.AddParameter("Streams", "Streams on which to run the test", [I3Frame.Physics])
			self.AddOutBox("OutBox")
			
			self.suites = []
			for case in test_cases:
				self.suites.append(unittest.defaultTestLoader.loadTestsFromTestCase(case))
			self.test_cases = test_cases
			
		def Configure(self):
			streams = self.GetParameter("Streams")
			for stream in streams:
				self.Register(stream, self.RunTests)
		
		def RunTests(self, frame):
			result = None
			
			for case in self.test_cases:
				case.frame = frame
			for test in self.suites:
				if result is None:
					result = self.runner.run(test)
				else:
					result = self.runner.run(test, result)
			
			if not result.wasSuccessful():
				# XXX TODO: is there a nicer way to pull the handbrake 
				# so that the failure shows up in runtests.py?
				raise AssertionError("A test failed!")
				self.RequestSuspension()
			else:
				self.PushFrame(frame)
		
		def Finish(self):
			pass
	
	return I3TestModule
