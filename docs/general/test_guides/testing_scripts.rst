.. highlight:: python

Testing Example Scripts
=======================

It's often very useful to simply test that your example scripts run.

.. warning::

    Please, please, please do not rely on these solely to increase test coverage.
    They test one and only one thing, namely, that your example script runs without
    throwing.

Below is a simple example.  In IceTray there's a TestExampleScripts class to help out.

::

   #!/usr/bin/env python3
   
   import unittest
   from icecube.icetray import I3Test
   
   class TestSimpleExample(I3Test.TestExampleScripts):
   
       project_name = "icetray"
   
       def test_simple_example(self):
           self.run_example('simple_example.py')

    unittest.main()

Now a simple example with arguments passed to the script.  You can just pass
arguments to **run_example** same as you would on the command line.

::

  #!/usr/bin/env python3
  
  import os
  import unittest
  from icecube.icetray import I3Test
  
  class TestSimpleExample(I3Test.TestExampleScripts):
  
      project_name = "dataio"
  
      def test_strip(self):
          input_file = self.I3_TESTDATA + "/sim/corsika.F2K010001_IC59_slim.i3.gz"
          output_file = "./output.i3.gz"
          self.run_example('strip.py', input_file, output_file)
  
      def tearDown(self):
          '''
          Be nice and clean up after yourself.
          '''
          if os.path.exists("./output.i3.gz"):
              os.remove("./output.i3.gz")

  unittest.main()

A more modern and pythonic way to write the same test would be

::

  #!/usr/bin/env python3
  
  import os
  import unittest
  from tempfile import NamedTemporaryFile
  from icecube.icetray import I3Test
  
  class TestSimpleExample(I3Test.TestExampleScripts):
  
      project_name = "dataio"
  
      def test_strip(self):
          input_file = self.I3_TESTDATA + "/sim/corsika.F2K010001_IC59_slim.i3.gz"
          with NamedTemporaryFile(dir=os.getcwd(), suffix='.gz') as output_file:
              self.run_example('strip.py', input_file, output_file.name)
  
  unittest.main()

