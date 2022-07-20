#!/usr/bin/env python3

import os
import unittest
from tempfile import NamedTemporaryFile
from icecube.icetray import I3Test

class TestSimpleExample(I3Test.TestExampleScripts):

    project_name = "dataio"

    def test_strip(self):
        input_file = self.I3_TESTDATA + "/sim/Level2_IC86.2011_corsika.010281.001664.00.i3.bz2"
        with NamedTemporaryFile(dir=os.getcwd(), suffix='.gz') as output_file:
            self.run_example('strip.py', input_file, output_file.name)

    def test_scan(self):
        input_file = self.I3_TESTDATA + "/sim/Level2_IC86.2011_corsika.010281.001664.00.i3.bz2"
        self.run_example('scan.py', '-c', input_file)

unittest.main()
