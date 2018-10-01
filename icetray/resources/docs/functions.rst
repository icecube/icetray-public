.. _pyfunction_as_module:

Passing python functions to :meth:`I3Tray.AddModule`
====================================================

:py::meth:`I3Tray.AddModule` has typically accepted only strings in
its first argument.  These strings are used to look up C++ classes
that inherit from :cpp:class:`I3Module`, like so::

  tray.AddModule('Dump', 'dump')

where the C++ module :cpp:class:`Dump` has been registered via the C++
:c:macro:`I3_MODULE()` macro.

:meth:`I3Tray.AddModule` now additionally accepts python functions in
its first argument, for instance::

  tray = I3Tray()

  def frame_printer(frame):
      print "Frame is:\n", frame

  tray.AddModule(frame_printer, 'printer')

When icetray receives a python function as the first argument to
``AddModule``, it constructs a special ``I3Module`` of type
*PythonFunction* which forwards the frames that it receives to the
python function passed.  By default it does this for the *Physics*
stream only.

.. warning::
   
   it is **not** 

      ``AddModule('frame_printer', ...``

   it is 	

      ``AddModule(frame_printer, ...``

   If you see a message like

      RuntimeError: Module/service "frame_printer" not registered with I3_MODULE() or I3_SERVICE_FACTORY()
   
   it is because you've put the python function into quotes, making it a string, and 
   icetray is failing to find that string in its registry of available C++ I3Modules.


If you put that function between a :class:`BottomlessSource`, (which
just pushed empty physics frames), the output should look like this::

  Frame is:
  [ I3Frame :
  ]

  Frame is:
  [ I3Frame :
  ]

Let's add a second function that puts something into the frame, and modify 
the frame_printer function to get and print it.

.. code-block:: python

   def int_putter(frame):
       frame['some_int'] = icetray.I3Int(777)

   tray.AddModule(int_putter, 'putter')


   def frame_printer(frame):
       print "Frame is:\n", frame
       value = frame['some_int'].value
       print "Value of int at some_int is", value

   tray.AddModule(frame_printer, 'printer')

Here the function :py:func:`int_putter` puts :c:type:`I3Int` with
the value 777 in into the frames as they go by.  This is reflected in
the table of contents printed by the :py:func:`frame_printer` function.

Output::

  Frame is:
  [ I3Frame :
    'some_int' ==> I3Int
  ]
  Value of int at some_int is 777

  Frame is:
  [ I3Frame :
    'some_int' ==> I3Int
  ]
  Value of int at some_int is 777

Functions with parameters
-------------------------

To be useful, reusable and modular, such functions need to take
parameters such as the location in the frame of useful frame objects,
values, thresholds, etc.  The hardcoded values *777* and *some_int*
just make our code brittle.

Functions passed to :func:`AddModule` may take more than one parameter
(the first parameter is always the :class:`I3Frame` that is flowing
through the framework).  The parameter values passed to
:func:`AddModule` will be delivered (along with the current
:class:`I3Frame`, of course) to the keyword parameters of the
associated python function passed each time the function is executed.

We modify the function :func:`int_putter` to accept parameters that
specify what value to put inside the :class:`I3Int`, and where in the 
frame to put them:

.. code-block:: python

   def int_putter(frame, where = 'someplace', value = -1):
       frame[where] = icetray.I3Int(value)

   tray.AddModule(int_putter, 'putter',
		  where = 'some_int',
		  value = 777)

   def frame_printer(frame, whatvalue):
       print "Frame is:\n", frame
       value = frame[whatvalue].value
       print "Value of int at", whatvalue, "is", value

   tray.AddModule(frame_printer, 'printer',
		  whatvalue = 'some_int')

Note the default parameter values for the function :func:`int_putter`.

Direct Usage of Lambda Functions
--------------------------------

Here we use a *lambda*, (nameless inline) function. Lambda functions
are also called lambda expressions because they can only contain simple
expressions. Note that functions created with lambda expressions cannot 
contain statements (if, while, for, try, with, ...). Check google for
more information on this standard python construct.

This makes writing very short modules possible. A simple function::

   def int_putter(frame):
       frame['some_int'] = icetray.I3Int(777)

   tray.AddModule(int_putter, 'putter')

can become the single line::

   tray.AddModule(lambda fr: fr['a_int'] = icetray.I3Int(777), 'putter')


Choosing streams the functions should run on 
--------------------------------------------

The underlying PythonFunction module also takes a parameter *Streams*,
which is a list of stream types that the function should run on.  By
default this list is ``[icetray.I3Frame.Physics]``.  To e.g. cause a
python function :func:`foo` to run on ``Calibration`` and ``Geometry``
streams, configure as follows::

   from icecube import icetray
	 
   def foo(frame):
       ...  # do something physicsy here

   tray.AddModule(foo, 'foofunc',
                  Streams = [icetray.I3Frame.Geometry,
                             icetray.I3Frame.Calibration])


Functions as filters
--------------------

The functions passed to :func:`AddModule` may return ``None``
(i.e. never call ``return`` at all), or a boolean.  The
:class:`PythonFunction` module examines the return values of these
functions and if the value is ``None`` or ``True``, the module will
call :func:`PushFrame`: modules further down the chain will see the
frame.  If the function returns ``False``, the module will drop the
frame.

.. note::

   The rationale for having ``None`` and ``True`` correspond to the
   same action (typically ``None`` is taken to be ``False``), is so
   that the 'default' behavior (when nothing is returned) is
   reasonable.  Otherwise one- or two-line functions that just check
   or print data would need to have lines ``return True`` added.  The
   thinking is that this extra work to provoke behavior that should be
   default isn't so elegant.  So the rule of thumb is, if you want to
   drop the frame, return ``False``, otherwise don't bother returning
   anything (or return ``True`` if it is clearer to do so).

For instance, the following code would cause frames that contain
an :class:`I3Int` with value less than 80 to be dropped::

   def ints_are_greater_than(frame,  key,  threshold):
       frameval = frame[key].value
       return frameval > threshold

   tray.AddModule(ints_are_greater_than,
                  key = 'intlocation',
                  threshold = 80)


.. _conditionalmodulefns:

Passing python functions to I3ConditionalModules
------------------------------------------------

The old way
^^^^^^^^^^^

Recall that an :class:`I3ConditionalModule` looks for an
:class:`I3IcePick` in its :class:`I3Context`, indexed by string.  So
the user must configure an :class:`I3IcePickInstaller<T>` (where *T*
is the class containing the desired pick logic) and the name given by
the user to the instance of this pick logic must match the name that
the using module accesses it by.::

  tray.AddService('I3IcePickInstaller<I3FrameObjectFilter>', 'fofilter')(
      ("FrameObjectKey", 'some_int')
      )

  tray.AddModule('AddNulls', 'adder')(
      ('IcePickServiceKey', 'fofilter'),
      ('where', ['x1', 'x2', 'x3'])
      )
    
Here the module *AddNulls*, being an :class:`I3ConditionalModule`,
will add nulls named 'x1', 'x2', and 'x3' to the frame when its
icepick, located in its context via the string 'fofilter', returns
true.  

This has several disadvantages:

* The logic that triggers the *AddNulls* module is separated from
  the configuration of the module itself
* There is the possibility for name collisions in the various
  :class:`I3Contexts`.  

If the condition is complicated, for instance the disjunction of two
other conditions, the syntax gets yet more verbose.

The new way
^^^^^^^^^^^

As of icetray v3, one can pass a python function to the parameter **If**
of I3ConditionalModules.  Identical to the above is the following::

  tray.AddModule('AddNulls', 'adder',
                 Where = ['x1', 'x2', 'x3'],
                 If    = lambda frame: 'some_int' in frame)

Another example:  run the reconstruction *LineFit* if the :class:`I3Int` at 
'where' is greater than 80::

   def ints_are_greater_than_80(frame):
       frameval = frame['where'].value         
       return frameval > 80

   tray.AddModule('LineFit', 'linefit',
                  HitSeries = 'WhereTheIntIs',
                  If = ints_are_greater_than_80)

Note that in this case the key in the frame and the value '80' are
hardcoded inside the python function we pass.  Not so good: we want to
reuse the functions we wrote in previous sections.  To do so we use a
small python forwarding function::

    def fwd(fn, **kwargs):
        def wrap(frame):
            return fn(frame, **kwargs)
        return wrap

Which captures the values of parameters passed to it and passes them on to the 
function ``fn``.  You would use this like this::

   def ints_are_greater_than(frame,  key,  value):
       frameval = frame[key].value
       return frameval > value

   tray.AddModule('LineFit', 'linefit',
                  If = fwd(ints_are_greater_than,
                           key = 'WhereTheIntIs',
                           value = 80))   

A forwarding function is necessary here, but not when passing a
python function directly to :func:`AddModule`.  This asymmetry is
unfortunate but presently unavoidable.

Functions as I3ConditionalModules
---------------------------------

Python functions now support the :class:`I3ConditionalModule` argument syntax,
with optional arguments **IcePickServiceKey** or **If**.  Use them exactly as
described above, or for another example, like this::

   def int_putter(frame):
       frame['other_int'] = icetray.I3Int(frame['some_int']*10)
   
   tray.AddModule(int_putter, 'putter',
                  If = lambda frame: 'some_int' in frame)

Source code organization
------------------------

You may want to store your useful functions in their own file, say my_utils.py::

   #
   #   my_utils.py
   #
   #   My useful stuff
   #    

   def ints_are_greater_than(frame,  key,  value):
       frameval = frame[key].value
       return frameval > value

Which should be located somewhere along your PYTHONPATH or in the current working 
directory.  To use them from your python scripts simply::

    #!/usr/bin/env python

    from my_utils import ints_are_greater_than
    from I3Tray import *

    tray = I3Tray()

    ...

    tray.AddModule(ints_are_greater_than, 'igt',
                   key = 'where',
                   value = 30)


