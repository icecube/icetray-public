Python scripting in IceTray
============================
Generally IceTray is controlled via a python script which sets up the needed 
modules and services, executes the Tray and cleans things up.  The
best illustration of this is a simple example::

 #!/usr/bin/env python
 #
 # Pass 2:
 
 from I3Tray import * 
 
 from os.path import expandvars
 
 import os
 import sys
 
 load("libicetray")
 load("libdataio")
 
 tray = I3Tray()
 
 tray.AddModule("I3Reader","reader", 
 		 Filename = "pass1.i3")

 tray.AddModule("Dump","dump")

 tray.AddModule("TrashCan","trash")
 
 tray.Execute()
 tray.Finish()

**TODO**  Flush out these docs more fully.

Configuring modules with vectors
--------------------------------
Python scripts can tersely configure
modules with vectors of ints, doubles, strings, and OMKeys.   
Project "examples" contains a script
"pass7_HelloConfiguration.py" which has an associated I3Module,
HelloConfiguration, which demonstrates the use of all this

In a python script, an individual OMKey is created like this:
OMKey(21,7).  you pass it to a module like you might expect::

 tray.AddModule("FryOM", "fry",
                OM = OMKey(3,3))


vectors of anythings are created by enclosing a comma separated
list of anythings in square-brackets::

 ["strings", "in", "a", "row"]

and you pass these to a module like this::

 tray.AddModule("ReverseTracks", "reverse",
                tracks = ["linefit", "dipolefit", "haystackfit"])

 tray.AddModule("OMSelect", "select",
                oms = [OMKey(1,2), OMKey(3,4), OMKey(5,6)])

Python details for experts
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

There's a gotcha. Python has lists, and it has tuples.  ithon will
convert either to an STL vector when it configures icetray, so this
syntax will work too::

 tray.AddModule("Jeopardy", "j")(
   ("I'll_take", "famous_numbers"),
   ("for", 500), 
   ("famous_numbers", (0, 1, 3.14159, 1.618034))
   )

here you're implicitly creating a list: notice the parenthesis.  But
if you want to configure a vector with only one entry, it wont work::

   ("famous_numbers", (3.14159))

here the parenthesis just disappear and that becomes just a double,
fand you get a "cant convert parameter" error when configuring.  But
tuples stay tuples::

   ("famous_numbers", [3.14159])

This works, and configures with a vector of length 1 containing pi.
So just use [square brackets].  They mean the same thing and they're
easier to see.


Running multiple I3Trays in a loop, in the same script
--------------------------------------------------------
In response to a recent email about running trays in a loop in a script, 
like this::

  for i in range(10):
     tray = I3Tray()
     tray.AddModule("foo") ...
 
     tray.Execute()

They would get through several iterations and then die.::

 >
 > My hypothesis was (but I can't say for sure) that python wasn't cleaning
 > up the I3Tray completely, and that when the service was deleted from the
 > previous file, it was accessing instead the memory from the current
 > file. 
 >
 
It is neither python's nor icetray's fault.  
To create and destroy I3Trays multiple
times in the same script (what you're doing in that loop) requires
that all objects/modules/services/everything that the I3Tray creates
leak no memory and clean themselves up correctly when they are
destroyed.  We know for sure this is not the case with ROOT objects
(they both leak memory and store state in globals and statics) and it
might very well be the case that some of our other modules don't do
this cleanly.  This is something we don't test, but should... on the
other hand it is likely that it would never really work well.  The
I3Tray should probably just prohibit this kind of thing.

Problems with global state are consistent with getting three or four
times through the loop and then crashing.  At some point the planets
align, some object accesses global state that is left over from the
previous iteration, and kaboom.  That adding a sleep() after each 
iteration helps a bit also makes sense, since if ROOT (or mysql or 
the SuchAndSuchService) has threads running in the background doing 
garbage collection (or perhaps simply
garbage redistribution), that 1 second pause would get you a few more
iterations before the crash.  

So one would need to examine all that code for statics/globals and be
sure that they restart well.  I'm not familiar with most of the code
you're using, but I bet there are lots of problems with this.

There's a pretty easy fix that is guaranteed to work, though: you
simply run each tray in a separate process and check the exit status
of the process.  Nonzero means failure.  One way to do this is to have
the python script fork() just before you create the I3Tray.  The
parent waits for the child to exit and collects the exit status of the
child process, and the child runs the tray and exits.  This uses the
operating system itself to guarantee any/all global state in the tray
gets cleaned up after each run::

 #!/usr/bin/env python
 
 import os, sys, time
 
 # load icetray libraries here
 
 for i in range(10): # for each of the jobs you're going to do
    # here you go to your database,
    # figure out what job is next, calculate parameters, etc
    pid = os.fork()                                         
    if pid:        
        # now there are *two* processes, and we're in the parent
        print "parent waiting...."
        (pid, status) = os.wait()
        if status == 0:
            print "parent notices that succeeded, marks database"
            # mark job as success in database
        else:
            print "parent notices that FAILED, marks accordingly"
            # mark job failed in database
    else:
        # we're in the child.  What we do here is invisible to the parent.
        # create and run actual I3Tray.  
        # tray = I3Tray(), blah blah
        # tray.Execute()
        time.sleep(1)
        print "child running icetray job i=%u" % i
        time.sleep(1)
        print "child running icetray job i=%u" % i
        time.sleep(1)
        print "child running icetray job i=%u" % i
        # simulate failure on every other process
        exitstatus = i%2
        if i%2 != 0:
            print "child is gonna throw an exception to simulate icetray having thrown an exception"
            raise "child oh noooo!"
        else:
            print "if child gets to here, nothing bad happened, return zero"
            sys.exit(0)


.. _NaN:
.. index:: NaN

.. _Inf:
.. index:: Inf

NaN and Inf
===========

:file:`I3Tray.py` creates namespace-scope variables ``Inf`` and
``Nan``, for the special floating-point values infinity and
not-a-number.  These are for the occasions when one needs to pass NaN
or Inf to a module parameter, or to initialize e.g. an :class:`I3Double <icecube.dataclasses.I3Double>`
to NaN.

``NaN`` compared to anything (including itself) is always false.  To
tell if a variable is nan, or not, use the functions ``isnan`` and
``isinf``.  In python 2.6 and later, these are in module :mod:`math`,
in earlier versions they are in module :mod:`scipy`::

  Python 2.6.2 (release26-maint, Apr 19 2009, 01:58:18) 
  [GCC 4.3.3] on linux2
  Type "help", "copyright", "credits" or "license" for more information.
  >>> from I3Tray import NaN, Inf
  >>> NaN
  nan
  >>> Inf
  inf
  >>> NaN == NaN       # You probably do *not* want this
  False
  >>> import math
  >>> math.isnan(NaN)  # You want this
  True
  >>> math.isinf(Inf)
  True
  


