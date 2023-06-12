Testing I3Modules
*****************
Below is a simple example of how to create a python unittest TestCase and run it
as a module in an I3Tray.

::

 #!/usr/bin/env python3
 
 import unittest
 from icecube.icetray import I3Tray
 from icecube import icetray
  
 class MyTestCase(unittest.TestCase):
     def test_frame_object_exists(self):     
         self.assert_("MyObject" in self.frame, "MyObject not found in the frame.")
 
 tray = I3Tray()
 tray.Add('I3Reader', Filename = "some_input_file.i3.bz2')
 tray.Add(icetray.I3TestModuleFactory(MyTestCase), Streams=[icetray.I3Frame.DAQ])
 tray.Execute()
 
