#!/usr/bin/env python

import os
import unittest
from icecube.icetray import I3Test

class TestSimpleExample(I3Test.TestExampleScripts):

    project_name = "dataio"

    def test_strip(self):
        input_file = self.I3_TESTDATA + "/sim/Level2_IC86.2011_corsika.010281.001664.00.i3.bz2"
        output_file = "./output.i3.gz"
        self.run_example('strip.py', input_file, output_file)

    def test_scan(self):
        input_file = self.I3_TESTDATA + "/sim/Level2_IC86.2011_corsika.010281.001664.00.i3.bz2"
        self.run_example('scan.py', '-c', input_file)

    def tearDown(self):
        '''
        Be nice and clean up after yourself.
        '''
        if os.path.exists("./output.i3.gz"):
            os.remove("./output.i3.gz")

unittest.main()
