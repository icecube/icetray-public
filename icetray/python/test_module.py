# -*- coding: utf-8 -*-
#
# (c) 2011
# the IceCube Collaboration
#
# $Id: test_module.py 1635 2013-06-28 21:33:24Z nwhitehorn $
#
# @version $Revision: 1635 $
# @date $LastChangedDate: 2013-06-28 17:33:24 -0400 (Fri, 28 Jun 2013) $
# @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy: nwhitehorn $
#
import unittest

from icecube.icetray import I3ConditionalModule, I3Frame


def I3TestModuleFactory(*test_cases):
    """Test case factory

    Create an `I3Module` that runs the given `unittest` test cases on
    each Physics frame.

    Example
    -------
    Create a simple test case::

        class I3TimeHorizonCutTest(unittest.TestCase):
            def testKeys(self):
                # All keys are present.
                self.assertTrue(
                    "InIceRawData" in self.frame,
                    "Up is down, black is white.")

                self.assertTrue(
                    "InIceRawData_Horizon" in self.frame,
                    "Up is down, black is white.")

            def testSizeReduction(self):
                # Output map is ever so slightly smaller.
                inlaunches = self.frame["InIceRawData"]
                outlaunches = self.frame["InIceRawData_Horizon"]
                self.assert_(len(outlaunches) <= len(inlaunches))

        def execute_test(frame):
            return "InIceRawData" in frame and len(frame["InIceRawData"]) > 0

        tray.Add(
            I3TestModuleFactory(I3TimeHorizonCutTest),
            If=execute_test)

    """
    class I3TestModule(I3ConditionalModule):
        test_runner = unittest.TextTestRunner()

        def __init__(self, context):
            I3ConditionalModule.__init__(self, context)

            self.AddParameter(
                "Streams",
                "Streams on which to run the test",
                [I3Frame.Physics])

            self.AddOutBox("OutBox")

            self.test_suites = None
            self.test_cases = test_cases

        def Configure(self):
            streams = self.GetParameter("Streams")

            for stream in streams:
                self.Register(stream, self.RunTests)

        def RunTests(self, frame):
            # Pass the frame to each test case.
            for case in self.test_cases:
                case.frame = frame
            if self.test_suites is None:
                self.test_suites = unittest.TestSuite([
                    unittest.defaultTestLoader.loadTestsFromTestCase(c)
                    for c in test_cases
                    ])

            test_result = self.test_runner.run(self.test_suites)

            # On some Python versions (e.g. 3.6),running the tests is
            # destructive for the suite and the suite needs to be reset
            self.test_suites = None

            if not test_result.wasSuccessful():
                # XXX TODO: is there a nicer way to pull the handbrake so that
                # the failure shows up in runtests.py?
                raise AssertionError("A test failed!")
                self.RequestSuspension()
            else:
                self.PushFrame(frame)

    return I3TestModule
