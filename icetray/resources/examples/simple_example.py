#!/usr/bin/env python

'''
This script doesn't really do much.  For output, you should
see 10 frames with an object of unkown type called 'my_object'
like so:

[ I3Frame  (Physics):
  'my_object' [Physics] ==> I3FrameObject (unk)
]

'''

from icecube import icetray
from I3Tray import I3Tray

class SimplePhysicsSource(icetray.I3Module):
    '''
    This is an example of a simple source module.
    It creates frames and pushes them to downstream modules.
    '''
    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        
    def Process(self):
        frame = icetray.I3Frame(icetray.I3Frame.Physics)
        self.PushFrame(frame)

class AddStuffToFrames(icetray.I3Module):
    '''
    This is an example of a module that adds an object
    to the physics frame.
    '''
    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        
        self.AddParameter("frame_object_name",
                          "Name of the frame object to add",
                          "my_object")
        
    def Configure(self):
        self.frame_object_name = self.GetParameter("frame_object_name")
        
    def Physics(self, frame):
        fo = icetray.I3FrameObject()
        frame[self.frame_object_name] = fo
        self.PushFrame(frame)

# You can use lamba functions as modules
# This is an illustration of a simple filter.
filter_frames = lambda frame : frame.Has('my_object')
        
def print_frames(frame):
    '''
    This is an example of a "function as a module."
    You just need to write a function that takes a frame
    and it can be inserted into the tray just like a module.
    '''
    print(frame)


tray = I3Tray()
tray.AddModule(SimplePhysicsSource)
tray.AddModule(AddStuffToFrames)
tray.AddModule(filter_frames)
tray.AddModule(print_frames)
tray.Execute(10)
