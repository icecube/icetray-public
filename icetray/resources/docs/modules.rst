I3Modules in python
===================

Dump
----

The *Dump* module just prints the table of contents of the frames that
it receives.  We'll implement this module in python and then expand on
it to make it do something useful.

We start by importing project *icetray*::

  from icecube import icetray

and define a class that inherits from :class:`I3Module`, with a
do-nothing constructor, (the ``__init__`` function).  It forwards to
the underlying constructor of :class:`I3Module` and takes, the same as
in C++, one argument which is an :class:`I3Context`.  The
:func:`Configure` method does nothing interesting, and the
:func:`Physics` method just prints and pushes the frame that it is
passed::


  class PyDump(icetray.I3Module):

      def __init__(self, context):
          icetray.I3Module.__init__(self, context)

      def Physics(self, frame):
          print frame
          self.PushFrame(frame)

one puts all of this into a file :file:`PyDump.py`, and uses it as
follows::

   #!/usr/bin/env python

   from PyDump import PyDump
   from I3Tray import *

   tray = I3Tray()

   tray.AddModule('BottomlessSource')
   
   tray.AddModule(PyDump)

   tray.Execute(10)

When you run this script, you should see ten empty frames go by.

Parameter handling
------------------

Python I3Modules add and get parameters very similar to C++ I3Modules.
Here is a module that puts :class:`I3Int` objects into the frames 
that go by, with consecutive increasing values::
 
  from icecube import icetray

  class PutInts(icetray.I3Module):

      def __init__(self, context):
          icetray.I3Module.__init__(self, context)
	  self.AddParameter('Where',                     # name
                            'Where to put those ints',   # doc
                            'somewhere')                 # default

	  self.AddParameter('StartWith',                 # name
                            'Start at this value',       # doc
                            0)                           # default

      def Configure(self):
          self.where = self.GetParameter('Where')
          self.value = self.GetParameter('StartWith')

      def Physics(self, frame):
          newint = icetray.I3Int(self.value)    # create the int
	  frame[self.where] = newint            # put it in the frame
          self.PushFrame(frame)                 # push the frame
          self.value += 1                       # increment our value

the values of the parameters specified with :func:`AddParameter` and
:func:`GetParameter` are passed to the tray the same as for C++
modules.   The call to :func:`AddModule` takes the python *class* 
object, not an instance of the class.  If we've stored the module
above in a file :file:`PutInts.py`::

   from PutInts import PutInts
   from icecube import icetray # (etc)

   ...

   tray.AddModule(PutInts, 'pi',
                  Where = 'intval_location',
                  StartWith = 13)

.. note::

   Similar to functions, note that we pass the bare python object to
   the I3Tray, not its name in a string, as with C++ modules.   i.e.
   it isn't this::
   
       tray.AddModule('PutInts', 'pi', ...

   it is this::

       tray.AddModule(PutInts, 'pi', ....

   and that symbol :class:`PutInts` of course must be known to the script,
   typically via a call to :func:`import`.

.. _paramtypes:

Allowable parameter types for python modules (any!)
---------------------------------------------------

One effect of the integration of python and C++ in icetray is that
python I3Modules can take parameters of any type.  For instance, this
module takes a python dictionary as an argument, using the keys in the
dictionary as frame locations to store I3Doubles, whose values are
the values found in the dictionary::

  class MultiAdder(I3Module):
      def __init__(self, context):
	  I3Module.__init__(self, context)
	  self.AddParameter("values", "key/value pairs to put into the frame", None)

      def Configure(self):
	  self.d = self.GetParameter("values")
	  print ">>>>> Configured with", self.d

      def Physics(self, frame):
	  for (k,v) in self.d.items():
	      i = icetray.I3Int(v)
	      frame.Put(k, i)
	  self.PushFrame(frame)

You pass the python dictionary parameter to the tray in the same way
you would pass any other parameter.  Here we pass it as a literal::

  tray.AddModule(MultiAdder, "mod",
		 values = { 'one' : 1,
			    'two' : 2,
			    'three' : 777 })
 
Putting the :class:`MultiAdder` module between a
:class:`BottomlessSource` and a :class:`Dump`, you should see frames going by 
that look like this::

  [ I3Frame :
    'one' ==> I3Int
    'three' ==> I3Int
    'two' ==> I3Int
  ]

Parameters can be input/output
------------------------------

Python objects like lists have identity.  That is, if I create a dictionary
that two python identifiers point to, and change the dictionary via one identifier, 
the other will see the change:

.. code-block:: pycon

    >>> d = { 'one' : 1, 'two' : 2 }
    >>> e = d
    >>> e
    {'two': 2, 'one': 1}
    >>> e['three'] = 3
    >>> e
    {'three': 3, 'two': 2, 'one': 1}
    >>> d
    {'three': 3, 'two': 2, 'one': 1}
  
which makes it easy to extract and collect values from the run of a
tray, via its parameters (this is considerably cleaner, and even less
trouble, than doing it via globals).  This module extracts and
collects the values of passing I3Ints in the frame::

  class IntCollector(I3Module):
      def __init__(self, context):
	  I3Module.__init__(self, context)
	  self.AddParameter("where", "where to get the ints from", None)
	  self.AddParameter("dest", "where to put the collected values", None)

      def Configure(self):
	  self.where = self.GetParameter("where")
	  self.dest = self.GetParameter("dest")

      def Physics(self, frame):
      	  self.dest.append(frame[self.where].value)
	  self.PushFrame(frame)

when configuring this module, we would pass an empty list, referenced by an 
existing identifier, to the ``dest`` parameter::

   dest_list = []
   #
   # add modules to get data from somewhere here
   #

   tray.AddModule(IntCollector, 'collect',
                  where = 'i3int_location',
                  dest = dest_list)

   tray.Execute()

   print "The values we collected are:", dest_list

of course to pass a literal empty list to the IntCollector module does us no good,
though it is legal::

   tray.AddModule(IntCollector, 'collect',
                  where = 'i3int_location',
                  dest = [])

as we have no way to access the data when the tray has finished
executing.  The possibilities here are quite large: you could pass
functions to modules, frame objects like :class:`I3Geometry` ...  feel
free to get messy.




 

      


 




       

   
  


   






   
