#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import os
import unittest
from tempfile import NamedTemporaryFile
from icecube.icetray import I3Test
from icecube import dataio,dataclasses

class TestSimpleExample(I3Test.TestExampleScripts):

    project_name = "phys-services"

    def test_event_hunter(self):
        input_file = self.I3_TESTDATA + "/sim/corsika.F2K010001_IC59_slim.i3.gz"
        with NamedTemporaryFile(dir=os.getcwd(), suffix=".gz") as output_file:
            self.run_example('event_hunter.py',
                             '--event=2,29,83',
                             '--output='+output_file.name,
                             input_file)
            ids = set()
            for f in dataio.I3File(output_file.name):
                ids.add(f['I3EventHeader'].event_id)
        self.assertEqual(ids,set([2,29,83]),'did not get all ids. found:'+str(ids))

    def test_audit_gcd(self):
        input_file = self.I3_TESTDATA + "/2023data/Level2_IC86.2023_data_Run00138821_84_781_GCD_withSLCcal2.i3.zst"
        with NamedTemporaryFile(dir=os.getcwd(), suffix=".i3.zst") as output_file:
            self.run_example('AuditGCD.py',
                             '-i',
                             input_file)

unittest.main()
