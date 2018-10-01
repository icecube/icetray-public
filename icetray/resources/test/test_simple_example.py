#!/usr/bin/env python

import unittest
from icecube.icetray import I3Test

class TestSimpleExample(I3Test.TestExampleScripts):

    project_name = "icetray"
    
    def test_simple_example(self):
        self.run_example('simple_example.py')

unittest.main()
