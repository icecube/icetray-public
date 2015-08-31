#!/usr/bin/env python

import unittest
from icecube import icetray

class test_I3ModuleFactory(unittest.TestCase):

    def test_get_projects(self):
        projects = icetray.projects()
        self.assertEqual( len(projects), 1, "got the wrong number of projects")
        self.assertEqual( projects[0], 'icetray', "this is not the project you're looking for.")

unittest.main()
            
        

        
