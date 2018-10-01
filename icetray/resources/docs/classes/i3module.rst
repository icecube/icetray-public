.. index:: 
   pair: I3Module; C++ Class
.. _I3Module:

.. highlight:: pycon

I3Module Reference
====================


Writing an I3Module
--------------------------
The standard building block of icecube offline analysis software is the I3Module. If you're new to icecube software, the best way to get started is to get some data and write an I3Module to sift through it.

Registering interest in one or more streams
--------------------------------------------

Icetray uses a 'frame-stream-stop' model (explained in detail elsewhere). The streams are Geometry, Calibration, DetectorStatus, DAQ, and Physics. An I3Module tells icetray which streams it is interested in by implementing the virtual functions Geometry(I3FramePtr), Calibration(I3FramePtr), DetectorStatus(I3FramePtr), DAQ(I3FramePtr) or Physics(I3FramePtr).

The I3_MODULE()  macro
-----------------------

Take a small section from a typical steering file::

 tray.AddModule('I3SimpleSeed', 'seed', SeedZenith=1.28, SeedAzimuth=math.pi)

This tells icetray that it should create an I3Module of type I3SimpleSeed, add it to the tray, and configure a couple of the I3SimpleSeed's parameters. In order to accomplish this, icetray needs to be able to convert the string "I3SimpleSeed" to a function that creates a module of type "I3SimpleSeed".

The mechanics of all this are hidden from the user. One simply registers one's module with the framework by calling the I3_MODULE() macro with the name of the object to be created. This call belongs at the top of the module's implementation file, after the #include directives.

For instance the example module I3SimpleSeed, above, would have, in I3SimpleSeed.cxx::

 class I3SimpleSeed : public I3Module { ... };

 I3_MODULE(I3SimpleSeed);   
 
 I3SimpleSeed::I3SimpleSeed(const I3Context& context)
 {
   ...

Modules that are instances of a template class specify the instance::

 I3_MODULE(I3IcePickModule<I3EventListFilter>);

The name passed to I3_MODULE() is the name one uses to refer to modules of this type in steering files. e.g. with the code above, one would be able to call::

 tray.AddModule('I3IcePickModule<I3EventListFilter>', 'eventpick',
     # etc
     )

Modules can be 'renamed', from the perspective of the steering file writer. If one wanted a different name for the 'I3PickModule<I3EventListFilter>', (or any other module) one could alias it with a 'typedef'::

 typedef I3PickModule<I3EventListFilter> I3EventPick;
 I3_MODULE(I3EventPick);

I3ConditionalModule
---------------------
A framework for conditional execution of a module is available in IceTray by subclassing I3ConditionalModule instead of I3Module.  I3ConditionalModule adds a new parameter to your module ('If') that can be set to python function in the configuration that determines whether the module is run on a particular frame. If the function returns 'True', the module will be run, and otherwise it will be skipped.

Adding Conditional Execution to Your Module
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you have an existing module that you'd like to add the possibility of conditional execution, just change these lines::

 #include "icetray/I3Module.h"
 
 ... snip ...
 
 class I3MyRecoModule : public I3Module
 {
 
 ... snip ...
 
 I3MyRecoModule::I3MyRecoModule(const I3Context& ctx) : I3Module(ctx)

to these lines::

 #include "icetray/I3ConditionalModule.h"
 
 ... snip ...
 
 class I3MyRecoModule : public I3ConditionalModule
 {
 
 ... snip ...
 
 I3MyRecoModule::I3MyRecoModule(const I3Context& ctx) : I3ConditionalModule(ctx)


Doing all this will add the parameter If to your module.  The default setting for this parameter is 'None', and leaving the parameter like this will result in normal operation of your module.  That way, existing scripts will operate as usual.

Using Conditional Execution
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Simple conditions can be specified inline as python lambda functions. This one runs the module only if its seed is present in the frame::

 tray.AddModule('I3MyRecoModule', 'myreco', SomeParameter=0,
     AnotherParameter=False, Seed='SeedParticle',
     If=lambda frame: 'SeedParticle' in frame)

Complicated Conditionals
^^^^^^^^^^^^^^^^^^^^^^^^^

More complicated conditionals can be specified using longer functions. For example, to run 'I3MyRecoModule' only when the seed particle exists, has a zenith angle greater than 90 degrees, and the number of hit channels is greater than 15::

 def condition(frame):
     if len(frame['InIceRawData']) <= 15:
        return False
     return frame.Has('SeedParticle') and frame['SeedParticle'].dir.zenith > \
        90*I3Units.degree
 tray.AddModule('I3MyRecoModule', 'myreco', SomeParameter=0,
     AnotherParameter=False, Seed='SeedParticle',
     If=condition)

I3PacketModule
--------------

There is one additional I3Module subclass you can inherit from: I3PacketModule. This module processes frames in packets instead of one at a time. Each packet is defined by a sentinel frame followed by zero or more frames of another type; the usual use case is to simultaneously process one DAQ frame along with event Physics frame in the event.

Instead of calling Physics() or DAQ(), I3PacketModule subclasses implement a method called FramePacket(), which is passed an array of I3FramePtrs instead of a single one. The module can then modify the frames, and then must push them to the next module, just like a regular I3Module.

Here is a small example of such a module, written in Python. It operates on packets of DAQ frames and Physics frames and then writes the number of Physics frames in the event to the DAQ frame (the first one)::

 class ExampleMod(icetray.I3PacketModule):
    def __init__(self, context):
        I3PacketModule.__init__(self, context)

    def FramePacket(self, frames):
        i = icetray.I3Int(len(frames) - 1)
        frames[0].Put('NSplits', i)
        for fr in frames:
            self.PushFrame(fr)

 tray.AddModule(ExampleMod, 'mod')

All I3PacketModules can be run conditionally, much like I3ConditionalModule subclasses, except that the function passed to If must operate on a python list of frames instead of on a single one. For instance, to run the above module only on those events triggering more than 15 DOMs::

 tray.AddModule(ExampleMod, 'mod',
      If=lambda frs: len(frs[0]['InIceRawData']) > 15)

This uses the InIceRawData key in the first frame of the packet (the DAQ frame) to control processing for the entire packet.

