.. _moving_to_v3_faq:

Moving to IceTray v3 FAQ
========================

While we've made every effort to make the transition from
older releases of IceTray to the V3 releases, there are a few 
places where some adjustments to your projects and scripts will be
needed.

IceTray modules
---------------

I3Tray considers the first module added to be the 'driving' module.
This should be a reader or generator, i.e. a module that does not access
its inbox.  In the past the semantics of this were a little fuzzier
as modules had a half-implemented scheme whereby they could add
inboxes.  (At some point it was decided that modules may have one and
only one inbox).

Tool python must be explicitly listed in USE_TOOLS
--------------------------------------------------

In older releases, the tool python was often implicitly included
in the build system, but this is no longer the case for increased 
flexibility. 

If you're getting an error like this::

 In file included from <command line>:5:
 /home/user/offline-trunk/build/CMakeFiles/I3.h:23:24: pyconfig.h: No such file or directory
 In file included from /home/user/tools/include/boost-1.38.0/boost/python/detail/prefix.hpp:13,
 from /home/user/tools/include/boost-1.38.0/boost/python/extract.hpp:8, from
 /home/user/offline-trunk/src/icetray/public/icetray/I3Configuration.h:30, 
   from /home/user/offline-trunk/src/icetray/public/icetray/I3Module.h:32, from
 /home/user/offline-trunk/src/icetray/public/icetray/I3ConditionalModule.h:24,

Then you need to make sure you've got the 'python' entry in your USE_TOOLS
line in your CMakeLists.txt.

Stream name argument to modules need additional qualifier
----------------------------------------------------------

Some small updates to your scripts if you ever pass a list of streams
to your module as a parameter.

For example::

 tray.AddModule("I3Writer","Writer")(
     ("filename","Output.i3"),
     ("streams",["Physics"])
     )

Would be changed to::

 tray.AddModule("I3Writer","Writer")(
     ("filename","Output.i3"),
     ("streams",[icetray.I3Frame.Physics])
     )

Full details are available in :ref:`pyfunction_as_module`.
