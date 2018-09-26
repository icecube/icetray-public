#!/usr/bin/env python

import os
import unittest
from icecube.icetray import I3Test
from icecube import dataio,dataclasses

class TestSimpleExample(I3Test.TestExampleScripts):

    project_name = "phys-services"

    def test_event_hunter(self):
        input_file = self.I3_TESTDATA + "/sim/corsika.F2K010001_IC59_slim.i3.gz"
        output_file = "./output.i3.gz"
        self.run_example('event_hunter.py',
                         '--event=2,29,83',
                         '--output='+output_file,
                         input_file)
        ids = set()
        for f in dataio.I3File(output_file):
            ids.add(f['I3EventHeader'].event_id)
        self.assertEqual(ids,set([2,29,83]),'did not get all ids. found:'+str(ids))

    def tearDown(self):
        '''
        Be nice and clean up after yourself.
        '''
        if os.path.exists("./output.i3.gz"):
            os.remove("./output.i3.gz")

unittest.main()
