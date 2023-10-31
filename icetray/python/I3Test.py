import os
import subprocess
import unittest
from pathlib import PurePath


class TestExampleScripts(unittest.TestCase):
    '''
    This is a helper class to make it really easy for project maintainers to
    write tests for their example scripts.

    NB : You must set self.project_name = "<project_name>"

    You get I3_TESTDATA for free, since many examples might need this.

    Here's a simple example of how to use this:

        #!/usr/bin/env python3

        from icecube.icetray import I3Test

        class TestSimpleExample(I3Test.TestExampleScripts):

            project_name = "icetray"

            def test_simple_example(self):
                self.run_example('simple_example.py')

        unittest.main()


    '''

    def setUp(self):
        self.I3_TESTDATA = os.path.expandvars("$I3_TESTDATA")

    def run_example(self, example_name, *args):
        p = PurePath(os.path.expandvars("$I3_BUILD"))
        p = p / self.project_name / "resources/examples"
        p = p / example_name
        script = str(p)
        del p

        cmd = [script]
        for arg in args:
            cmd.append(arg)
        print(cmd)
        return_code = subprocess.call(cmd)
        if return_code != 0:
            print(f"Script returned error code {return_code}")
        self.assertEqual(return_code, 0, f"Example {script} did not finish successfully.")
