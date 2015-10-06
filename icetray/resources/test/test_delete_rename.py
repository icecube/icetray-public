#!/usr/bin/env python

import unittest
from icecube import icetray
from I3Tray import I3Tray

class test_I3FrameObject_delete_rename(unittest.TestCase):

    def setUp(self):
        self.tray = I3Tray()
    
        def add_frame_object(frame):
            # any frame object will do
            frame["FrameObject"] = icetray.OMKey(21,30)

        self.tray.AddModule("BottomlessSource")
        self.tray.AddModule(add_frame_object)

    # Uncomment the line below when we get unittest2 on the bots
    # ...and remove the try-except block
    #@unittest.expectedFailure
    def test_delete_rename(self):
        self.tray.AddModule("Delete",keys="FrameObject")
        self.tray.AddModule("Rename", keys=["FrameObject","OMKey"])
        try:
            self.tray.Execute(10)
        except Exception:
            pass

    # Uncomment the line below when we get unittest2 on the bots
    # ...and remove the try-except block
    #@unittest.expectedFailure
    def test_rename_delete(self):
        self.tray.AddModule("Rename", keys=["FrameObject","OMKey"])
        self.tray.AddModule("Delete",keys="FrameObject")
        try:
            self.tray.Execute(10)
        except Exception:
            pass

unittest.main()
            
        

        
