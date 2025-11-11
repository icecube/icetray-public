#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2025 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import os
import unittest
from icecube.icetray import I3Test

class TestSimpleExample(I3Test.TestExampleScripts):

    project_name = "tableio"

    def test_book_data(self):
        '''
        Test that runs the one example script for rootwriter.
        '''
        gcd_file = self.I3_TESTDATA + "/GCD/GeoCalibDetectorStatus_2013.56429_V1.i3.gz"
        input_file = self.I3_TESTDATA + "/sim/Level2_IC86.2011_corsika.010281.001664.00.i3.bz2"
        self.run_example('rootwriter_book_data.py', 'output.root', gcd_file, input_file)

    def tearDown(self):
        '''
        Be nice and clean up after yourself.
        '''
        os.remove("output.root")
        
unittest.main()
