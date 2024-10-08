.. SPDX-FileCopyrightText: 2024 The IceTray Contributors
..
.. SPDX-License-Identifier: BSD-2-Clause

Python scripting in IceTray
============================

Generally IceTray is controlled via a python script which sets up the needed
modules and services, executes the Tray and cleans things up.  The
best illustration of this is a simple example::

 #!/usr/bin/env python3
 #
 # Pass 2:

 import os
 import sys
 from os.path import expandvars
 from icecube.icetray import I3Tray
 from icecube import icetray, dataio

 tray = I3Tray()
 tray.AddModule("I3Reader","reader",
 		 Filename = "pass1.i3")
 tray.AddModule("Dump","dump")
 tray.Execute()

**TODO**  Flush out these docs more fully.

Configuring modules with vectors
--------------------------------

In a Python script, an individual :cpp:class:`OMKey` is created like this:
``OMKey(21,7)``.  You pass it to a module like you might expect::

 tray.AddModule("FryOM", "fry",
                OM = OMKey(3,3))

Vectors of "things" are created by enclosing a comma separated list of
"things" in square-brackets::

 ["strings", "in", "a", "row"]

and you pass these to a module like this::

 tray.AddModule("ReverseTracks", "reverse",
                tracks = ["linefit", "dipolefit", "haystackfit"])

 tray.AddModule("OMSelect", "select",
                oms = [OMKey(1,2), OMKey(3,4), OMKey(5,6)])

Python details for experts
^^^^^^^^^^^^^^^^^^^^^^^^^^

There's a gotcha. Python has lists, and it has tuples.  Python will
convert either to an STL vector when it configures icetray, so this
syntax will work too::

 tray.AddModule("Jeopardy",
   I'll_take="famous_numbers",
   for=500,
   famous_numbers=(0, 1, 3.14159, 1.618034)
   )

here you are creating a tuple: notice the parenthesis.  But if you
want to configure a vector with only one entry, it won't work::

   famous_numbers=(3.14159)

here the parenthesis is just ignored and that becomes just a double,
and you get a "can't convert parameter" error when configuring.  To
tell Python that this is supposed to be tuple, use this syntax::

   famous_numbers=(3.14159,)

which is a bit awkward.  We recommend to use lists, where this issue
does not occur::

   famous_numbers=[3.14159]

This works, and configures with a vector of length 1 containing pi.
So just use [square brackets].  They mean the same thing and they're
easier to see.

.. _NaN:
.. index:: NaN

.. _Inf:
.. index:: Inf

NaN and Inf
===========

:file:`I3Tray.py` creates namespace-scope variables ``Inf`` and
``Nan``, for the special floating-point values infinity and
not-a-number.  These are for the occasions when one needs to pass NaN
or Inf to a module parameter, or to initialize e.g. an :cpp:type:`I3Double`
to NaN.

``NaN`` compared to anything (including itself) is always false.  To
tell if a variable is NaN, or not, use the functions :py:func:`~math.isnan` and
:py:func:`~math.isinf`.  In Python 2.6 and later, these are in module :mod:`math`,
in earlier versions they are in module :mod:`scipy`:

.. code-block:: pycon

  >>> from icecube.icetray import NaN, Inf
  >>> NaN
  nan
  >>> Inf
  inf
  >>> NaN == NaN       # You definitely *do not* want this
  False
  >>> import math
  >>> math.isnan(NaN)  # You want this
  True
  >>> math.isinf(Inf)
  True
