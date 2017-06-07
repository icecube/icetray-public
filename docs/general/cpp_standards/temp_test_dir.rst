Temporary Test Directories
==========================

Always use a temporary directory for testing.  This prevents writing files
directly to the base metaproject directory, which could cause conflicts
with other tests or parts of the codebase.

Python Script
-------------

For python script tests, set up a temporary directory like such::

    import os
    import tempfile
    import shutil
    
    orig_dir = os.getcwd()
    tmp_dir = tempfile.mkdtemp(dir=orig_dir)
    os.chdir(tmp_dir)
    try:
        # do tests here
        
    finally:
        os.chdir(orig_dir)
        shutil.rmtree(tmp_dir)

Unit Test
---------

If you are using the :py:mod:`unittest` framework, there is an alternate way::

    import os
    import tempfile
    import shutil

    class MyTest(unittest.TestCase):
        def setUp(self):
            super(MyTest,self).setUp()
            
            orig_dir = os.getcwd()
            tmp_dir = tempfile.mkdtemp(dir=orig_dir)
            os.chdir(tmp_dir)
            def clean_dir():
                os.chdir(orig_dir)
                shutil.rmtree(tmp_dir)
            self.addCleanup(clean_dir)
        
        def test_me(self):
            # do test here like normal
