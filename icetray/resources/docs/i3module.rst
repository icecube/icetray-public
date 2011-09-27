.. index:: 
   pair: I3Module; C++ Class
.. _I3Module:

.. highlight:: pycon

I3Module Reference
====================


Writing an I3Module
--------------------------
The standard building block of icecube offline analysis software is the !I3Module. If you're new to icecube software, the best way to get started is to get some data and write an I3Module to sift through it.

'''TODO MORE DETAILS NEEDED'''
	
Registering interest in one or more streams
--------------------------------------------

Icetray uses a 'frame-stream-stop' model (explained in detail elsewhere). The streams are Geometry, Calibration, DetectorStatus, and Physics. An I3Module tells icetray which streams it is interested in by implementing the virtual functions Geometry(I3FramePtr), Calibration(I3FramePtr), DetectorStatus(I3FramePtr), or Physics(I3FramePtr).

'''TODO MORE NEEDED'''

The I3_MODULE()  macro
-----------------------

Take a small section from a typical steering file::

 tray.AddModule("I3SimpleSeed","seed")(
     ("SeedZenith",1.28),
     ("SeedAzimuth",3.14)
     )

This tells icetray that it should create an I3Module of type I3SimpleSeed, add it to the tray, and configure a couple of the I3SimpleSeed's parameters. In order to accomplish this, icetray needs to be able to convert the string "I3SimpleSeed" to a function that creates a module of type "I3SimpleSeed".

The mechanics of all this are hidden from the user. One simply registers one's module with the framework by calling the I3_MODULE() macro with the name of the object to be created. This call belongs at the top of the module's implementation file, after the #include directives.

For instance the example module I3SimpleSeed, above, would have, in I3SimpleSeed.cxx::

 #include <seedproject/I3SimpleSeed.h>

 I3_MODULE(I3SimpleSeed);   
 
 I3SimpleSeed::I3SimpleSeed(const I3Context& context)
 {
   ...

Modules that are instances of a template class specify the instance::

 I3_MODULE(I3IcePickModule<I3EventListFilter>);

The name passed to I3_MODULE() is the name one uses to refer to modules of this type in steering files. e.g. with the code above, one would be able to call::

 tray.AddModule("I3IcePickModule<I3EventListFilter>","eventpick")(
     # etc
     )

Modules can be 'renamed', from the perspective of the steering file writer. If one wanted a different name for the 'I3PickModule<I3EventListFilter>', (or any other module) one could alias it with a 'typedef'::

 typedef I3PickModule<I3EventListFilter> I3EventPick;
 I3_MODULE(I3EventPick);

I3ConditionalModule
---------------------
A framework for conditional execution of a module's physics method is available in IceTray.  It's done by adding another level of inheritance between a functioning module and icetray's I3Module.  This new class is I3ConditionalModule in the project icetray.  It adds a parameter for the name of an IcePick service that needs to be installed as a service in the context.  This IcePick service will then check frames as they come through, and return a 'True' for when the physics frame should be executed and 'False' for when it should be skipped.

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


Doing all this will add the parameter IcePickServiceKey to your module.  The default setting for this parameter is "".  Leaving the parameter like this will result in normal operation of your module.  That way, existing scripts will operate as usual.

Using Conditional Execution
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

First, you'll need to add an IcePick to the tray as a service.  This can be done with the I3IcePickInstaller, which is a service factory.  The syntax is as follows::

 tray.AddService("I3IcePickInstaller<I3NLaunchFilter>","nchan-pick")(
     ("DataReadoutName","InIceRawData"),
     ("MinNLaunch",12)
     )
 
 tray.AddModule("I3MyRecoModule", "myreco")(
     ("IcePickServiceKey","nchan-pick"),
     ("SomeParameter", 0),
     ("AnotherParameter", False)
     )

Make sure that the ServiceName and IcePickServiceKey parameters are matched up.  Note that it is possible to install one IcePick in the context, and to use the same one by many different conditional modules.

Complicated Conditionals
^^^^^^^^^^^^^^^^^^^^^^^^^

If the condition upon which you need to decide to execute is complicated, you can build up the final decision by using the I3IcePicks: I3AndServices and I3OrServices.  Note that these picks are fundamentally different from the I3AndFilter and I3OrFilter that you're probably used to.  The I3*Services picks look for I3IcePicks that are installed as services and work with them appropriately.  The two I3*Filter picks look for I3IcePick decisions that are bools sitting in the frame.

Anyway, using these I3IcePicks, you can do things like::

 tray.AddService("I3IcePickInstaller<I3NLaunchFilter>","nchan-pick")(
     ("DataReadoutName","InIceRawData"),
     ("MinNLaunch",12)
     )
 
 tray.AddService("I3IcePickInstaller<I3ZenithFilter>","zenith-pick")(
     ("ParticleKey","Linefit"),
     ("MinZenith",70*I3Units.degree)
     )
 
 tray.AddService("I3IcePickInstaller<I3AndServices>","nchan-and-zenith-pick")
    ("ServicesToAnd",["nchan-pick","zenith-pick"])
    )
 
 tray.AddModule("I3MyRecoModule", "myreco")(
     ("IcePickServiceKey","nchan-and-zenith-pick"),
     ("SomeParameter", 0),
     ("AnotherParameter", False)
     )

One thing to keep in mind about the I3IcePicks that are installed as services, is that when they are used by multiple modules, they will recalculate their decision for each module.  Because of this, an I3IcePick could end up being called multiple times for the same frame.  This could become a problem if your I3IcePick is based on some random numbers, or if it's a complicated, time-consuming algorithm.  For this reason, each I3IcePick now has a parameter "CacheResults" that is False by default.  When activated, an I3IcePick will save its decision in the frame as an I3Bool at a unique key.  Before executing, the I3IcePick will check for this I3Bool, and just use it instead of recalculating.  For example::

 tray.AddService("I3IcePickInstaller<I3NLaunchFilter>","nchan-pick")(
     ("CacheResults",True),
     ("DataReadoutName","InIceRawData"),
     ("MinNLaunch",12)
     )
 
 tray.AddModule("I3MyRecoModule", "reco")(
     ("IcePickServiceKey","nchan-pick"),
     ("SomeParameter", 0),
     ("AnotherParameter", False)
     )
 
 tray.AddModule("I3DifferentRecoModule", "a-different-reco")(
     ("IcePickServiceKey","nchan-pick"),
     ("ParameterToCheck", 0),
     ("AnotherInterestingParameter", False)
     )
 
When these modules are run, the I3IcePick's SelectFrame method will only see each frame once.  Frames will also have an I3Bool in them at the key "nchan-pick_cache".  '''Be very careful when saving these I3Bools to a file!!!'''  I would strongly suggest you don't, by using the "SkipKeys" parameter to the I3Writer, the Keep module, or the Delete module.  If these I3Bools are written to a file, and then read again with another I3IcePick of the same name with "CacheResults" set to True (but different parameters), that IcePick will never actually use it's SelectFrame method, and it's behavior would quietly be what you don't expect.  This seems like a tough situation to recreate, but it would be very difficult to debug.
