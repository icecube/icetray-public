How to run a python script under the debugger
---------------------------------------------

First be sure that you have built either **Debug** or
**RelWithDebInfo**.   In an icetray environment::

  % ./env-shell.sh
  ************************************************************************
  *                                                                      *
  *                   W E L C O M E  to  I C E T R A Y                   *
  *                                                                      *
  *              Version offline-software.trunk     r48553               *
  *                                                                      *
  *                You are welcome to visit our Web site                 *
  *                        http://icecube.umd.edu                        *
  *                                                                      *
  ************************************************************************

  Icetray environment has:
     I3_SRC       = /home/troy/Icecube/meta-projects/offline-software/trunk/src
     I3_BUILD     = /home/troy/Icecube/meta-projects/offline-software/trunk/build

run gdb using ``--args``, passing ``python`` and your script::

  % gdb --args python ./examples/resources/scripts/pass1.py 
  GNU gdb 6.8-debian
  Copyright (C) 2008 Free Software Foundation, Inc.
  License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
  This is free software: you are free to change and redistribute it.
  There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
  and "show warranty" for details.
  This GDB was configured as "i486-linux-gnu"...

I have added some segfaulting code to an example module.  Run the
script using **r**::

  (gdb) r
  Starting program: /usr/bin/python ./examples/resources/scripts/pass1.py
  [Thread debugging using libthread_db enabled]
  [New Thread 0xb7ddcad0 (LWP 18082)]
  Loading libicetray........................................ok
  Loading libdataclasses....................................ok
  Loading libphys-services..................................ok
  Loading libDOMcalibrator..................................ok
  Loading libdataio.........................................ok
  Loading libexamples.......................................ok
  i3reader:  Filename = /opt/i3/ports/test-data/2006data/Run00089508.i3.gz
  i3reader:  SkipKeys = ['I3PfFilterMask']
  writer:  filename = pass1.i3
  /home/troy/Icecube/meta-projects/offline-software/trunk/src/dataio/private/dataio/I3Reader.cxx:177: INFO : Opened file /opt/i3/ports/test-data/2006data/Run00089508.i3.gz
  /home/troy/Icecube/meta-projects/offline-software/trunk/src/dataio/private/dataio/I3WriterBase.cxx:113: INFO : Not compressing.
  Geometry.....[1]
  [ I3Frame  (Geometry):
    'I3Geometry' [Geometry] ==> I3Geometry(459538)
  ]
  ---------------------------------
  Calibration.....[2]
  [ I3Frame  (Calibration):
    'I3Calibration' [Calibration] ==> I3Calibration(11267753)
    'I3Geometry' [Geometry] ==> I3Geometry(459538)
  ]
  ---------------------------------
  DetectorStatus.....[3]
  [ I3Frame  (DetectorStatus):
    'I3Calibration' [Calibration] ==> I3Calibration(11267753)
    'I3DetectorStatus' [DetectorStatus] ==> I3DetectorStatus(96286)
    'I3Geometry' [Geometry] ==> I3Geometry(459538)
  ]
  ---------------------------------

  Program received signal SIGSEGV, Segmentation fault.
  [Switching to Thread 0xb7ddcad0 (LWP 18082)]
  0xb2dbca1c in DumbFeatureExtractor::Physics (this=0x83a5c58, frame=@0xbfdc6c00)
      at /home/troy/Icecube/meta-projects/offline-software/trunk/src/examples/private/modules/DumbFeatureExtractor.cxx:55
  55        printf("%d", *p);
  Current language:  auto; currently c++
  (gdb)

One of the modules has segfaulted and gdb has stopped execution and
given you a prompt.  Now it is useful to see a stack trace::

  (gdb) where
  #0  0xb2dbca1c in DumbFeatureExtractor::Physics (this=0x83a5c58, frame=@0xbfdc6c00)
      at /home/troy/Icecube/meta-projects/offline-software/trunk/src/examples/private/modules/DumbFeatureExtractor.cxx:55
  #1  0xb77afc73 in I3Module::Process (this=0x83a5c58)
      at /home/troy/Icecube/meta-projects/offline-software/trunk/src/icetray/private/icetray/I3Module.cxx:179
  #2  0xb77b0443 in I3Module::Do (this=0x83a5c58, f=&virtual I3Module::Process())
      at /home/troy/Icecube/meta-projects/offline-software/trunk/src/icetray/private/icetray/I3Module.cxx:113
  #3  0xb77b050b in I3Module::Do (this=0x83a4ed0, f=&virtual I3Module::Process())
      at /home/troy/Icecube/meta-projects/offline-software/trunk/src/icetray/private/icetray/I3Module.cxx:131
  #4  0xb77b050b in I3Module::Do (this=0x83a44f0, f=&virtual I3Module::Process())
      at /home/troy/Icecube/meta-projects/offline-software/trunk/src/icetray/private/icetray/I3Module.cxx:131
  #5  0xb779bbe6 in I3Tray::Execute (this=0x81ccd08)
      at /home/troy/Icecube/meta-projects/offline-software/trunk/src/icetray/private/icetray/I3Tray.cxx:381
  #6  0xb7c12230 in boost::python::detail::invoke<int, void (I3Tray::*)(), boost::python::arg_from_python<I3Tray&> > (f=@0x8352c14, 
      tc=@0xbfdc6df4) at /opt/i3/ports/include/boost-1.36.0/boost/python/detail/invoke.hpp:94
  #7  0xb7c14aee in boost::python::detail::caller_arity<1u>::impl<void (I3Tray::*)(), boost::python::default_call_policies, boost::mpl::vector2<void, I3Tray&> >::operator() (this=0x8352c14, args_=0x832ae0c)
      at /opt/i3/ports/include/boost-1.36.0/boost/python/detail/caller.hpp:223
  #8  0xb7c14b4a in boost::python::objects::caller_py_function_impl<boost::python::detail::caller<void (I3Tray::*)(), boost::python::default_call_policies, boost::mpl::vector2<void, I3Tray&> > >::operator() (this=0x8352c10, args=0x832ae0c, kw=0x0)
      at /opt/i3/ports/include/boost-1.36.0/boost/python/object/py_function.hpp:38
  #9  0xb73408e1 in boost::python::objects::py_function::operator() (this=0x8359040, args=0x832ae0c, kw=0x0)
      at /opt/i3/ports/var/db/dports/build/file._opt_i3_ports_var_db_dports_sources_rsync.code.icecube.wisc.edu_icecube-tools-ports_devel_boost_1.36.0/work/boost_1.36.0/libs/python/include/boost/python/object/py_function.hpp:143
  #10 0xb733f2d2 in boost::python::objects::function::call (this=0x83591e8, args=0x832ae0c, keywords=0x0)
      at /opt/i3/ports/var/db/dports/build/file._opt_i3_ports_var_db_dports_sources_rsync.code.icecube.wisc.edu_icecube-tools-ports_devel_boost_1.36.0/work/boost_1.36.0/libs/python/src/object/function.cpp:226
  #11 0xb733f3d1 in operator() (this=0xbfdc7000)
      at /opt/i3/ports/var/db/dports/build/file._opt_i3_ports_var_db_dports_sources_rsync.code.icecube.wisc.edu_icecube-tools-ports_devel_boost_1.36.0/work/boost_1.36.0/libs/python/src/object/function.cpp:581
  #12 0xb733f3f0 in invoke (function_obj_ptr=@0xbfdc6fe0)
      at /opt/i3/ports/var/db/dports/build/file._opt_i3_ports_var_db_dports_sources_rsync.code.icecube.wisc.edu_icecube-tools-ports_devel_boost_1.36.0/work/boost_1.36.0/boost/function/function_template.hpp:179
  #13 0xb7355cb7 in boost::function0<void>::operator() (this=0xbfdc6fdc)
      at /opt/i3/ports/var/db/dports/build/file._opt_i3_ports_var_db_dports_sources_rsync.code.icecube.wisc.edu_icecube-tools-ports_devel_boost_1.36.0/work/boost_1.36.0/boost/function/function_template.hpp:815
  #14 0xb73545f0 in boost::python::handle_exception_impl (f=@0xbfdc6fdc)
      at /opt/i3/ports/var/db/dports/build/file._opt_i3_ports_var_db_dports_sources_rsync.code.icecube.wisc.edu_icecube-tools-ports_devel_boost_1.36.0/work/boost_1.36.0/libs/python/src/errors.cpp:25
  #15 0xb733e177 in handle_exception<boost::python::objects::<unnamed>::bind_return> (f=
	{m_result = @0xbfdc7030, m_f = 0x83591e8, m_args = 0x832ae0c, m_keywords = 0x0})
      at /opt/i3/ports/var/db/dports/build/file._opt_i3_ports_var_db_dports_sources_rsync.code.icecube.wisc.edu_icecube-tools-ports_devel_boost_1.36.0/work/boost_1.36.0/libs/python/include/boost/python/errors.hpp:29
  #16 0xb733e1ff in function_call (func=0x83591e8, args=0x832ae0c, kw=0x0)
      at /opt/i3/ports/var/db/dports/build/file._opt_i3_ports_var_db_dports_sources_rsync.code.icecube.wisc.edu_icecube-tools-ports_devel_boost_1.36.0/work/boost_1.36.0/libs/python/src/object/function.cpp:613
  #17 0x0805cb97 in PyObject_Call (func=0x2, arg=0x832ae0c, kw=0x0) at ../Objects/abstract.c:1861
  #18 0x080c7aa7 in PyEval_EvalFrameEx (f=0x83a4104, throwflag=0) at ../Python/ceval.c:3784
  #19 0x080cb1f7 in PyEval_EvalCodeEx (co=0xb7d6e2a8, globals=0xb7d69cec, locals=0x0, args=0x81a4bbc, argcount=1, kws=0x81a4bc0, 
      kwcount=0, defs=0x0, defcount=0, closure=0x0) at ../Python/ceval.c:2836
  #20 0x080c93fe in PyEval_EvalFrameEx (f=0x81a4a84, throwflag=0) at ../Python/ceval.c:3669
  #21 0x080cb1f7 in PyEval_EvalCodeEx (co=0xb7d679b0, globals=0xb7db6acc, locals=0xb7db6acc, args=0x0, argcount=0, kws=0x0, 
      kwcount=0, defs=0x0, defcount=0, closure=0x0) at ../Python/ceval.c:2836
  #22 0x080cb347 in PyEval_EvalCode (co=0xb7d679b0, globals=0xb7db6acc, locals=0xb7db6acc) at ../Python/ceval.c:494
  #23 0x080ea818 in PyRun_FileExFlags (fp=0x816b008, filename=0xbfdc931f "./examples/resources/scripts/pass1.py", start=257, 
  ---Type <return> to continue, or q <return> to quit---
      globals=0xb7db6acc, locals=0xb7db6acc, closeit=1, flags=0xbfdc7648) at ../Python/pythonrun.c:1273
  #24 0x080eaab9 in PyRun_SimpleFileExFlags (fp=0x816b008, filename=0xbfdc931f "./examples/resources/scripts/pass1.py", closeit=1, 
      flags=0xbfdc7648) at ../Python/pythonrun.c:879
  #25 0x08059335 in Py_Main (argc=1, argv=0xbfdc7714) at ../Modules/main.c:523
  #26 0x080587f2 in main (argc=Cannot access memory at address 0x2
  ) at ../Modules/python.c:23
  (gdb) 

OK, this is quite huge, but don't be discouraged.  Start at the top,
where the innermost function is listed, where the to-first-order most
useful information probably is.  In this case it is after icetray has
entered the ``I3Tray::Execute()`` method::

  (gdb) where
  #0  0xb2dbca1c in DumbFeatureExtractor::Physics (this=0x83a5c58, frame=@0xbfdc6c00)
      at /home/troy/Icecube/meta-projects/offline-software/trunk/src/examples/private/modules/DumbFeatureExtractor.cxx:55
  #1  0xb77afc73 in I3Module::Process (this=0x83a5c58)
      at /home/troy/Icecube/meta-projects/offline-software/trunk/src/icetray/private/icetray/I3Module.cxx:179
  #2  0xb77b0443 in I3Module::Do (this=0x83a5c58, f=&virtual I3Module::Process())
      at /home/troy/Icecube/meta-projects/offline-software/trunk/src/icetray/private/icetray/I3Module.cxx:113
  #3  0xb77b050b in I3Module::Do (this=0x83a4ed0, f=&virtual I3Module::Process())
      at /home/troy/Icecube/meta-projects/offline-software/trunk/src/icetray/private/icetray/I3Module.cxx:131
  #4  0xb77b050b in I3Module::Do (this=0x83a44f0, f=&virtual I3Module::Process())
      at /home/troy/Icecube/meta-projects/offline-software/trunk/src/icetray/private/icetray/I3Module.cxx:131
  #5  0xb779bbe6 in I3Tray::Execute (this=0x81ccd08)

Here you can see that the crashing function is
``DumbFeatureExtractor::Physics()``.  We can see where we are in the
function with **list**::

  (gdb) list
  50            }
  51          frame->Put(outputSeries_, inIceSeries);
  52        }
  53        
  54        int *p = 0;
  55        printf("%d", *p);
  56
  57        // ice top
  58        if(featureExtractIceTop_)
  59        {

Line 55 above is also what gdb reported to us when it first caught our
segfault.  I can examine the values of various variables::

  (gdb) print outputSeries_
  $1 = {static npos = 4294967295, 
    _M_dataplus = {<std::allocator<char>> = {<__gnu_cxx::new_allocator<char>> = {<No data fields>}, <No data fields>}, 
      _M_p = 0x83a57b4 "InIceRecoHitSeries"}}
  (gdb) print featureExtractIceTop_
  $2 = true
  (gdb) print p
  $2 = (int *) 0x0

And this would explain my segfault (that I am dereferencing a null
pointer).

Dealing with ``log_fatal()`` calls
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``log_fatal()`` is a function that throws an exception.  I add a ``log_fatal()`` before 
the crashing call to printf, but I don't give it an informative message::

  (gdb) r
  The program being debugged has been started already.
  Start it from the beginning? (y or n) y
  Starting program: /usr/bin/python ./examples/resources/scripts/pass1.py
  [Thread debugging using libthread_db enabled]
  [New Thread 0xb7d98ad0 (LWP 18183)]
  Loading libicetray........................................ok
  Loading libdataclasses....................................ok
  Loading libphys-services..................................ok
  Loading libDOMcalibrator..................................ok
  Loading libdataio.........................................ok
  Loading libexamples.......................................ok
  i3reader:  Filename = /opt/i3/ports/test-data/2006data/Run00089508.i3.gz
  i3reader:  SkipKeys = ['I3PfFilterMask']
  writer:  filename = pass1.i3
  /home/troy/Icecube/meta-projects/offline-software/trunk/src/dataio/private/dataio/I3Reader.cxx:177: INFO : Opened file /opt/i3/ports/test-data/2006data/Run00089508.i3.gz
  /home/troy/Icecube/meta-projects/offline-software/trunk/src/dataio/private/dataio/I3WriterBase.cxx:113: INFO : Not compressing.
  Geometry.....[1]
  [ I3Frame  (Geometry):
    'I3Geometry' [Geometry] ==> I3Geometry(459538)
  ]
  ---------------------------------
  Calibration.....[2]
  [ I3Frame  (Calibration):
    'I3Calibration' [Calibration] ==> I3Calibration(11267753)
    'I3Geometry' [Geometry] ==> I3Geometry(459538)
  ]
  ---------------------------------
  DetectorStatus.....[3]
  [ I3Frame  (DetectorStatus):
    'I3Calibration' [Calibration] ==> I3Calibration(11267753)
    'I3DetectorStatus' [DetectorStatus] ==> I3DetectorStatus(96286)
    'I3Geometry' [Geometry] ==> I3Geometry(459538)
  ]
  ---------------------------------
  /home/troy/Icecube/meta-projects/offline-software/trunk/src/examples/private/modules/DumbFeatureExtractor.cxx:56: FATAL: I am crashing here but not telling you why.
  Traceback (most recent call last):
    File "./examples/resources/scripts/pass1.py", line 81, in <module>
      tray.Execute()
    File "/home/troy/Icecube/meta-projects/offline-software/trunk/build/lib/I3Tray.py", line 74, in Execute
      args[0].the_tray.Execute()
  RuntimeError: I am crashing here but not telling you why.
				     merge:      1 calls to physics     13.00s user      0.00s system

  Program exited with code 01.
  (gdb) where
  No stack.

Here you can see that I get the filename, linenumber, and message
associated with the crash, but I have no stacktrace and cannot examine
the values/conditions that may have provoked this call to
``log_fatal()``.  This is because ``log_fatal()`` has thrown an
exception.  I can tell *gdb* to stop when an exception is thrown by
issuing *catch throw*::

  (gdb) catch throw
  Catchpoint 1 (throw)
  (gdb) r
  Starting program: /usr/bin/python ./examples/resources/scripts/pass1.py
  [Thread debugging using libthread_db enabled]
  [New Thread 0xb7d5fad0 (LWP 18185)]
  Loading libicetray........................................ok
  Loading libdataclasses....................................ok
  Loading libphys-services..................................ok
  Loading libDOMcalibrator..................................ok
  Loading libdataio.........................................ok
  Loading libexamples.......................................ok
  i3reader:  Filename = /opt/i3/ports/test-data/2006data/Run00089508.i3.gz
  i3reader:  SkipKeys = ['I3PfFilterMask']
  writer:  filename = pass1.i3
  /home/troy/Icecube/meta-projects/offline-software/trunk/src/dataio/private/dataio/I3Reader.cxx:177: INFO : Opened file /opt/i3/ports/test-data/2006data/Run00089508.i3.gz
  /home/troy/Icecube/meta-projects/offline-software/trunk/src/dataio/private/dataio/I3WriterBase.cxx:113: INFO : Not compressing.
  Geometry.....[1]
  [ I3Frame  (Geometry):
    'I3Geometry' [Geometry] ==> I3Geometry(459538)
  ]
  ---------------------------------
  Calibration.....[2]
  [ I3Frame  (Calibration):
    'I3Calibration' [Calibration] ==> I3Calibration(11267753)
    'I3Geometry' [Geometry] ==> I3Geometry(459538)
  ]
  ---------------------------------
  DetectorStatus.....[3]
  [ I3Frame  (DetectorStatus):
    'I3Calibration' [Calibration] ==> I3Calibration(11267753)
    'I3DetectorStatus' [DetectorStatus] ==> I3DetectorStatus(96286)
    'I3Geometry' [Geometry] ==> I3Geometry(459538)
  ]
  ---------------------------------
  /home/troy/Icecube/meta-projects/offline-software/trunk/src/examples/private/modules/DumbFeatureExtractor.cxx:56: FATAL: I am crashing here but not telling you why.
  [Switching to Thread 0xb7d5fad0 (LWP 18185)]

  Catchpoint 1 (exception thrown)
  0xb73b7e05 in __cxa_throw () from /usr/lib/libstdc++.so.6
  (gdb)

And I see that gdb has stopped me inside the exception-throwing
function.  This isn't immediately helpful::

   (gdb) list
   ../Modules/python.c: No such file or directory.  in
   ../Modules/python.c

But I can ask for a stacktrace::

  (gdb) where
  #0  0xb73b7e05 in __cxa_throw () from /usr/lib/libstdc++.so.6
  #1  0xb2d3fbfb in DumbFeatureExtractor::Physics (this=0x83a5c58, frame=@0xbfd20360)
      at /home/troy/Icecube/meta-projects/offline-software/trunk/src/examples/private/modules/DumbFeatureExtractor.cxx:56
  #2  0xb7732c73 in I3Module::Process (this=0x83a5c58)
      at /home/troy/Icecube/meta-projects/offline-software/trunk/src/icetray/private/icetray/I3Module.cxx:179
  #3  0xb7733443 in I3Module::Do (this=0x83a5c58, f=&virtual I3Module::Process())
      at /home/troy/Icecube/meta-projects/offline-software/trunk/src/icetray/private/icetray/I3Module.cxx:113
  #4  0xb773350b in I3Module::Do (this=0x83a4ed0, f=&virtual I3Module::Process())
      at /home/troy/Icecube/meta-projects/offline-software/trunk/src/icetray/private/icetray/I3Module.cxx:131
  #5  0xb773350b in I3Module::Do (this=0x83a44f0, f=&virtual I3Module::Process())
      at /home/troy/Icecube/meta-projects/offline-software/trunk/src/icetray/private/icetray/I3Module.cxx:131
  #6  0xb771ebe6 in I3Tray::Execute (this=0x81ccd08)

and I see I need to move up the stack from ``__cxa_throw()`` to the site of the throw::

  (gdb) up
  #1  0xb2d3fbfb in DumbFeatureExtractor::Physics (this=0x83a5c58, frame=@0xbfd20360)
      at /home/troy/Icecube/meta-projects/offline-software/trunk/src/examples/private/modules/DumbFeatureExtractor.cxx:56
  56          log_fatal("I am crashing here but not telling you why.");
  (gdb) list
  51          frame->Put(outputSeries_, inIceSeries);
  52        }
  53        
  54        int *p = 0;
  55        if (!p)
  56          log_fatal("I am crashing here but not telling you why.");
  57        printf("%d", *p);
  58
  59        // ice top
  60        if(featureExtractIceTop_)

If you know that your program will throw (e.g. you're diagnosing a
problem that manifests itself via ``log_fatal()``, the *catch throw* 
might not work at first::

  % gdb --args python ./examples/resources/scripts/pass1.py
  GNU gdb 6.8-debian
  Copyright (C) 2008 Free Software Foundation, Inc.
  License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
  This is free software: you are free to change and redistribute it.
  There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
  and "show warranty" for details.
  This GDB was configured as "i486-linux-gnu"...
  (gdb) catch throw
  Function "__cxa_throw" not defined.
  (gdb) 

But after the first time you have run your program, gdb will know that there
exists an exception-throwing function and will allow you to catch there::

  (gdb) r
  Starting program: /usr/bin/python ./examples/resources/scripts/pass1.py
  [Thread debugging using libthread_db enabled]
  [New Thread 0xb7d8aad0 (LWP 18191)]
  Loading libicetray........................................ok
  Loading libdataclasses....................................Quit
    [ etc etc... get dumped back to the prompt ]

  (gdb) catch throw
  Catchpoint 1 (throw)
  (gdb) r



Profiling C++ Code
^^^^^^^^^^^^^^^^^^
Profilers show you how much time your code is spending in each function. The "80/20 rule" is that on average your code will spend 80% of its time in 20% of the code. This means of course that only 20% of the code is really worth optimizing. We tend to be extremely bad at predicting which 20% that will be.

When one optimizes code, one makes a decision that the loss of clarity introduced by rearranging code is worth the increase in speed that it brings. One therefore wants to do it only when and where necessary.
 
Furthermore, do not ever optimize code that is not thoroughly, I mean thoroughly tested:
   "It is much, much easier to make correct code fast than it is to make fast code correct."

Use the utilities **valgrind** (specifically the callgrind tool) and **kcachegrind** (nice graphical tool for viewing results).  Google them, you'll find stuff.
