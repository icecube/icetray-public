#!/usr/bin/env python3

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

unittest.main()
