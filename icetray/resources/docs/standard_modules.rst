Standard I3Modules supplied with IceTray
========================================

The standard modules supplied with icetray are generally small,
utility-type modules.

Copy
----

A module to copy one frame object to another, new location.

Example
^^^^^^^

.. highlight:: python

::

  tray.AddModule("Copy", "copy", 
                 Keys = ["src1", "dst1", "src2", "dst2"])
  

Delete
------

A module to remove specific items from the Frame.

Example
^^^^^^^

.. highlight:: python

::

  tray.AddModule("Delete", "delete1",
                  Keys = ["here", "there"])


Dump
----

Print the contents of the Frame for each and every stop seen

Example
^^^^^^^

.. highlight:: python

::

  tray.AddModule("Dump","Dump")

Keep
----

This module will clean out the frame.  Only frame object names provided in the
"Keys" argument will be kept.


Example
^^^^^^^

.. highlight:: python

::
  tray.AddModule('Keep', 'k',
                  Keys=['GoodStuff'])

Rename
------

Rename one frame object to a new, unique name

Example
^^^^^^^

.. highlight:: python

::

  tray.AddModule("Rename", "rename",
                  Keys =  ["here", "here2", "there", "there2"])


DeleteUnregistered
------------------

This module will loop through every frame it sees (on all streams) and
attempt to deserialize/*Get* every frameobject.  If any are
unregistered **or have unsupported version**, they will be dropped
from the frame.

This is useful if, for instance, a subsequent module iterates through
all frame objects and checks to see if they are some certain type: if
*DeleteUnregistered* has been run first, these modules can be
confident that none of the ``frame->Get<I3FrameObject>()`` operations
will throw.  

When might that operation throw?  When you're reading data from a file
with the ``I3Reader``, and that file contains data written by a module
that isn't currently loaded (e.g. there are ``I3LineFitParams``
objects in there, and you haven't loaded the ``linefit`` project).

.. highlight:: cpp

Here is an example loop::

  void LoopingModule::Physics()
  {
    I3FramePtr frame = PopFrame();
    
    vector<string> keys;
    keys = frame->keys();

    for (int i=0; i<keys.size(); i++)
    {
       // get the object out:  may trigger deserialization and cause
       // an unregistered_class to be thrown.  If DeleteUnregistered
       // was run first, you can be sure 
       I3FrameObjectConstPtr fo_ptr = frame->Get<I3FrameObjectConstPtr>(keys[i]);
       I3IntConstPtr i_ptr = dynamic_pointer_cast<I3Int>(fo_ptr);
       if (i_ptr)
         log_info("Yes, %s is an I3Int", keys[i].c_str());
    }
    PushFrame(frame, "OutBox"); 
  }


Example
^^^^^^^
.. highlight:: python

::

  tray.AddModule("I3Reader","reader",
               Filename = "data.i3.gz")

  tray.AddModule("DeleteUnregistered", "deleter")


Get
---

This module does what DeleteUnregistered does, but instead of deleting
problematic frame objects, it allows the exceptions to propagate
upwards.  Useful for testing, to verify that everything in the frame
is deserializable.

Parameters
^^^^^^^^^^

**Keys**:

  only get these keys.  If unset, get everything.

**Streams**:

  A list of streams. Get on these streams.  If unset, get on all.

Example
^^^^^^^
::

  tray.AddModule("I3Reader","reader",
                 Filename = "data.i3.gz")

  tray.AddModule("Get", "getter",
                 Streams = [icetray.I3Frame.Physics,
                            icetray.I3Frame.DetectorStatus])



  





       

   
  


   






   


