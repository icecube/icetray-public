import sys
import unittest
import os
import unittest
import subprocess

# The following three should be removed once we
# use unitest.TestCase everywhere.  Gonna take some time.

def ENSURE(cond, msg):
	if not cond :
		print(msg)
		print("FAIL")
		sys.exit(1)

def ENSURE_EQUAL(x,y, msg):
	if not x == y :
		print(msg)
		print("FAIL")
		sys.exit(1)

def ENSURE_DISTANCE(x,y, d, msg):
	if abs(x - y) > d :
		print(msg)
		print("FAIL")
		sys.exit(1)

class TestExampleScripts(unittest.TestCase):
    '''
    This is a helper class to make it really easy for project maintainers to
    write tests for their example scripts.

    NB : You must set self.project_name = "<project_name>"
    
    You get I3_TESTDATA for free, since many examples might need this.

    Here's a simple example of how to use this:

        #!/usr/bin/env python

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

        script = "%s/%s/%s" %  (os.path.expandvars("$I3_BUILD"),
                                "%s/resources/examples" % self.project_name,
                                example_name)

        cmd = [script]
        for arg in args:
            cmd.append(arg)
        print(cmd)
        return_code = subprocess.call(cmd)
        if return_code != 0 :
                print("Script returned error code %d" % return_code)
        self.assertEqual(return_code, 0,
                         "Example %s did not finish successfully." % script)

