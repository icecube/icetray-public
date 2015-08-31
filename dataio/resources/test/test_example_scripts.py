#!/usr/bin/env python

import os
import unittest
from icecube.icetray import I3Test

class TestSimpleExample(I3Test.TestExampleScripts):

    project_name = "dataio"

    def test_strip(self):
        input_file = self.I3_TESTDATA + "/sim/corsika.F2K010001_IC59_slim.i3.gz"
        output_file = "./output.i3.gz"
        self.run_example('strip.py', input_file, output_file)

    def test_scan(self):
        input_file = self.I3_TESTDATA + "/sim/corsika.F2K010001_IC59_slim.i3.gz"
        output_file = "./output.i3.gz"
        self.run_example('strip.py', input_file, output_file)

# Some day this will work
    def test_event_hunter(self):
        input_file = self.I3_TESTDATA + "/sim/corsika.F2K010001_IC59_slim.i3.gz"
        output_file = "./output.i3.gz"
        self.run_example('event_hunter.py',
                         '--event=0', 
                         '--output=output.i3.gz',
                         input_file)

    def tearDown(self):
        '''
        Be nice and clean up after yourself.
        '''
        if os.path.exists("./output.i3.gz"):
            os.remove("./output.i3.gz")
        
unittest.main()
