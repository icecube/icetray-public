Standard I3Modules supplied with IceTray
========================================

The standard modules supplied with icetray are generally small,
utility-type modules.

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

Parameters
^^^^^^^^^^
None.

OutBoxes
^^^^^^^^
One.

Example
^^^^^^^
.. highlight:: python

::

  tray.AddModule("I3Reader","reader",
               Filename = "data.i3.gz")

  tray.AddModule("DeleteUnregistered", "deleter")

  tray.AddModule("TrashCan", "trash")


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

  A list of streams. Get on these streams.  If unset, get on GCDP.


OutBoxes
^^^^^^^^

One

Example
^^^^^^^
::

  tray.AddModule("I3Reader","reader",
                 Filename = "data.i3.gz")

  tray.AddModule("Get", "getter",
                 Streams = [icetray.I3Frame.Physics,
                            icetray.I3Frame.DetectorStatus])

  tray.AddModule("TrashCan", "trash")


  





       

   
  


   






   


