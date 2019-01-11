User's Guide
------------

.. highlight:: sh 

`CMake <http://www.cmake.org/>`_ is a cross-platform build system which
generates build files for use by native build systems for different
platforms.  On UNIX-like systems it generates makefiles.  CMake provides 
easier per-project build configuration, a more standard build
environment across different platforms, additional testing infrastructure, 
and out-of-source compilation.

Out-of-source compilation uses a separate object directory for build
results while leaving the original source directory untouched.  This
makes it possible to perform multiple simultaneous builds for multiple
platforms from one single source directory, and to more easily see 
which files have changed when committing new code.

Building with cmake
^^^^^^^^^^^^^^^^^^^

To begin building with CMake start with a clean workspace.  Set your
``I3_PORTS`` and ``JAVA_HOME`` environment variables.  You'll also need
``LD_LIBRARY_PATH`` set to include the architecture-dependent directory
containing ``libzip.so`` for anything that will require Java (the
following examples use ``bash`` as an example)::

  $ export I3_PORTS=~/IceCube/i3tools
  $ export JAVA_HOME=/usr/java/j2sdk1.4.2
  $ export LD_LIBRARY_PATH=${JAVA_HOME}/jre/lib/amd64

With cmake one does "out-of-source" builds.  This makes it easier to
build several configurations against one set of source, and to be sure
that the build process does not modify the source itself (e.g. no
temporary files end up laying around in your checkout.  So create a
work directory and check out source to a directory called ``src``:


::

  mkdir -p ~/IceCube/meta-projects/offline-software/trunk
  cd ~/IceCube/meta-projects/offline-software/trunk
  svn co http://code.icecube.wisc.edu/svn/meta-projects/offline-software/trunk src

*Note the "src" on the end of the svn co command*

Create the object directory parallel to the source directory and
execute cmake in the object directory to create the build environment::

  $ pwd
  ~/IceCube/meta-projects/offline-software/trunk
  $ mkdir build
  $ ls
  build src
  $ cd build
  $ $I3_PORTS/bin/cmake ../src
  <lots of cmake output omitted>
  -- Configuring done
  -- Generating done
  -- Build files have been written to: ~/IceCube/meta-projects/offline-software/build
                    
At this point the contents of the object directory should look somewhat
like that of the source directory, except there isn't any source there, just lots of makefiles::

  $ ls
  amanda-core            DartTestfile.txt  interfaces         pfreader
  analysis-tree          dataclasses       ithon              phys-services
  bin                    dataio            lib                tarball_hook.sh
  CMakeCache.txt         DOMcalibrator     log4cplus.conf     testheader.hpp.gch
  CMakeFiles             env-shell.sh      Makefile           Testing
  cmake_install.cmake    eventviewer       make_tarball.sh    triggerUtil-C++
  coordinate-service     examples          monolith-reader    twrclasses
  CTestCustom.cmake      I3Db              mutineer           twr-decode
  daq-decode             icepick           payloads-unpacker  TWRReader
  DartConfiguration.tcl  icetray           pfclasses
  $ ls ../src
  amanda-core            dataclasses       ithon              pfclasses
  analysis-tree          dataio            lib                pfreader
  bin                    DOMcalibrator     local-config.mk    physics-docs
  build                  env-shell.sh      log4cplus.conf     phys-services
  cmake                  eventviewer       Makefile           tarball_hook.sh.in
  CMakeLists.txt         examples          meta-project-docs  triggerUtil-C++
  coordinate-service     I3Db              monolith-reader    twrclasses
  CTestConfig.cmake      icepick           mutineer           twr-decode
  daq-decode             icetray           offline-mk         TWRReader
  DartConfig.cmake       interfaces        payloads-unpacker


While still in the build directory execute ``make``::

  $ pwd
  ~/IceCube/meta-projects/offline-software/build
  $ make


After ``make`` returns your build results will be in the
object directory.  Assuming no builds are made inside of the original
source directory that directory can be reused for other builds, even if
the platforms or build configurations differ.

The ``make`` target ``help`` will provide a list of
valid targets in the CMake-generated ``Makefile``::

  $ pwd
  ~/IceCube/meta-projects/offline-software/build
  $ make help
  The following are some of the valid targets for this Makefile:
  ... all (the default if no target is provided)
  ... clean
  ... depend
  <more targets omitted>

.. _rebuild_cache: 

Configuring your build
^^^^^^^^^^^^^^^^^^^^^^

Editing the CMake cache
"""""""""""""""""""""""

Having run ``cmake`` and generated a ''build'' directory full of
makefiles, one sets build options by editing the cmake cache.  You can
start the editor with "make edit_cache", or by invoking the cache
editor, ``ccmake``, directly::

  cd ~/Icecube/meta-projects/offline-software/trunk/build
  make edit_cache

A handy curses app will pop up allowing you to change the values of
various configuration variables.  As you move the cursor over them a
description is shown at the bottom of the window::

  BDB_FOUND                        OFF                           
  BDB_INCLUDE_DIR                  BDB_INCLUDE_DIR-NOTFOUND      
  BDB_LIBRARIES                                                  
  BOOST_FOUND                      ON                            
  BOOST_INCLUDE_DIR                /Users/troy/ports/include/boos
  BOOST_LIBRARIES                  /Users/troy/ports/lib/boost-1.
  BUILD_AMANDA-CORE                ON                            
  BUILD_ANALYSIS-TREE              ON                            
  BUILD_COORDINATE-SERVICE         ON                            
  BUILD_DAQ-DECODE                 ON                            
  BUILD_DATACLASSES                ON                            
  BUILD_DATAIO                     ON                            
  BUILD_DOMCALIBRATOR              ON                            
  BUILD_EXAMPLES                   ON                            
  BUILD_I3DB                       ON                            

  BDB_FOUND: BDB found successfully                                
  Press [enter] to edit option          CMake Version 2.4 - patch 6
  Press [c] to configure
  Press [h] for help         Press [q] to quit without generating
  Press [t] to toggle advanced mode (Currently Off)

 
After you have set things as you like them press **c** to run the
configuration.  *You may have to do this twice*.  When cmake feels
ready to generate makefiles, an option **g** will appear in the menu.
Press **g** to generate the new makefiles and exit.

Command line configuration
""""""""""""""""""""""""""

If you know what you want to set, you can also just do so from the
command line.  For instance, I have checked out source to directory
``src/`` and am in a parallel directory ``build/``.  I want to build
the "release" variant, the one with optimizations on, i issue::

   cmake -DCMAKE_BUILD_TYPE:STRING=Release ../src

You will notice that the *type* of the variable is represented after
the colon.  Almost everything can be handled with ``STRING`` and
``BOOL``.  For ``BOOL``, use the values ``ON`` and ``OFF``.

Not all of the variables are useful.  Here are some that are:

.. _USE_GFILT: use_gfilt
.. _CMAKE_BUILD_TYPE:
.. _Release:
.. index:: 
   pair: CMAKE_BUILD_TYPE; Release
.. _Debug:
.. index:: 
   pair: CMAKE_BUILD_TYPE; Debug
.. _RelWithDebInfo:
.. index:: 
   pair: CMAKE_BUILD_TYPE; RelWithDebInfo

.. list-table:: Useful configuration variables 
   :header-rows: 1
   
   * - Option
     - Description
     - Default
   * - BUILD\_\ *PROJECT*
     - Toggles the build of project *PROJECT*.
       e.g. ``BUILD_ICETRAY``, ``BUILD_DATAIO``.  One should appear
       per project in the current workspace.
     - ``ON``  	   
   * - .. attribute:: USE_CCACHE
     - Use ccache (if detected)
     - ON
   * - .. attribute:: USE_DISTCC
     - Use distcc (if detected)
     - OFF
   * - .. attribute:: USE_GFILT
     - Use the gfilt stl error message decryptor
     - ON
   * - .. attribute:: CMAKE_BUILD_TYPE
     - What kind of build you want.  The choices are ``Release``, ``Debug``, ``RelWithDebInfo``, ``MinSizeRel``, or empty for none of the above.
     - ``Debug``

Verbose build
"""""""""""""

If you issue the ``ccmake`` command **t** the advanced variables
will be shown.  The most useful of these are:

===========================================  ======================
Option                                       Description
===========================================  ======================
.. attribute:: CMAKE_VERBOSE_MAKEFILE        Shows commands used while building.  Useful for debugging cmake.
===========================================  ======================

.. index:: VERBOSE

Though you can also make the build verbose at any time by adding
``VERBOSE=1`` to the command line::

  make dataio-pybindings VERBOSE=1

The cmake cache CMakeCache.txt
""""""""""""""""""""""""""""""

These cache variables are stored in a file ``CMakeCache.txt`` in the
build directory.  You may modify this file directly instead of using
``make edit_cache`` or the ``ccmake`` utility as above.

If you do so, cmake will detect that this file has been modified and
automatically regenerate your makefiles.  However if you add or remove
files that a glob should match, e.g.::


  i3_add_library(mylib private/mylib/*.cxx)


there is no way for cmake to know that a new file
(e.g. ``private/mylib/newfile.cxx`` exists that should match
``private/mylib/*.cxx``, as this regular expression no longer exists.
It has been expanded into e.g. ''private/mylib/file1.cxx
private/mylib/file2.cxx''.  There are two ways to handle this:

1. *Don't use globs:* list the files explicitly,
   e.g.::

     i3_add_library(mylib private/mylib/file1.cxx private/mylib/file2.cxx).  

   This way to add/remove files one edits a
   CMakeLists.txt and cmake will detect that the file has been
   modified, and regenerate the makefiles.

#. ``make rebuild_cache``: The make target *rebuild_cache* will
   cause the makefiles to be regenerated.  During this process the
   globs from each CMakeLists.txt will again be expanded and things
   will work.

.. note::

  **If you add or remove projects** you must run ``make
  rebuild_cache``.  This is because the list of projects in the
  workspace is, as with the lists of files above, collected with a
  glob.


.. * optimizations vs. debugging:  see `CMAKE_BUILD_TYPE`
.. * verbose builds, see `CMAKE_VERBOSE_MAKEFILES` 

.. _testdriver:

Testing
^^^^^^^

Projects that have test executables or scripts will have their tests
run when ``make test`` is issued.  Test binaries are found in
``$I3_BUILD/bin``, the workspace target *test-bins* will build these
test binaries.

There is also a generated file ``runtests.py`` in ``$I3_BUILD/bin``.
This is a test driver script that you can use to run individual tests
or groups of tests.::

  % ./bin/runtests.py --help
  Usage: runtests.py [options]

  Options:
    -h, --help            show this help message and exit
    -p PROJECTS, --project=PROJECTS
			  List of projects to run tests on
    -s, --scripts-only    Run only test scripts
    -u, --units-only      Run only compiled unit tests
    -v, --verbose         Show verbose output

For example, to run the test scripts for project dataio, issue::

    % ./bin/runtests.py -p dataio -s
    27 projects loaded.
    0:0  ...  dataio/scripts/a_nocompression.py                                   
    1:0  ...  dataio/scripts/b_default.py                                         


.. [wiki:CMakeAddProject   Adding a project]

.. [wiki:CMakeReference   CMakeLists.txt Reference]

