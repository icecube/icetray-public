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
``I3_PORTS`` or enable the CVMFS environment if it is available. (The
following examples use ``bash`` as an example)::

  $ export I3_PORTS=~/IceCube/i3tools

or::

  $ eval $(/cvmfs/icecube.opensciencegrid.org/py2-v1/setup.sh)

With CMake, one creates "out-of-source" builds.  This makes it easier to
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
execute ``cmake`` in the object directory to create the build environment::

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
                    
At this point the contents of the object directory should look
somewhat like that of the source directory.  Main differences:

* there are no sourcefiles in the subdirectories of ``build/``
* build contains directories ``CMakeFiles/``, ``bin/``, and ``lib/``
* build contains files ``CMakeCache.txt``, ``env-shell.sh``, 
  and ``tarball_hook.sh``.

::

  $ ls
  CMakeCache.txt  cmake_install.cmake  glshovel/         pfclasses/
  CMakeFiles/     coordinate-service/  icepick/          pfreader/
  DOMcalibrator/  daq-decode/          icetray/          phys-services/
  I3Db/           dataclasses/         interfaces/       tarball_hook.sh*
  Makefile        dataio/              jebclasses/       twr-decode/
  Testing/        docs/                lib/              twrclasses/
  amanda-core/    env-shell.sh*        log4cplus.conf    util/
  bin/            examples/            mutineer/
  cmake/          filter-tools/        payload-parsing/

And the source directory will have been untouched::

  $ ls ../src
  CMakeLists.txt@      dataclasses/   icepick/          pfclasses/
  DOMcalibrator/       dataio/        icetray/          pfreader/
  I3Db/                docs/          interfaces/       phys-services/
  amanda-core/         examples/      jebclasses/       tarball_hook.sh.in
  cmake/               filter-tools/  twr-decode/       util/ 
  coordinate-service/  mutineer/      twrclasses/
  daq-decode/          glshovel/      payload-parsing/  
  
While still in the build directory execute ``make``::

  $ pwd
  ~/IceCube/meta-projects/offline-software/build
  $ make

After ``make`` returns, your build results will be in the
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
makefiles, one sets build options by editing the CMake cache.  You can
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
configuration.  *You may have to do this twice*.  When CMake is
ready to generate makefiles, an option **g** will appear in the menu.
Press **g** to generate the new makefiles and exit.

Command line configuration
""""""""""""""""""""""""""

If you know what you want to set, you can also just do so from the
command line.  For instance, I have checked out source to directory
``src/`` and am in a parallel directory ``build/``.  I want to build
the "release" variant, the one with optimizations on, i issue::

   cmake -DCMAKE_BUILD_TYPE=Release ../src

Common CMake Settings
~~~~~~~~~~~~~~~~~~~~~
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
   * - .. attribute:: BUILD_PROJECT
     - Toggles the build of project *PROJECT*.
       e.g. ``BUILD_ICETRAY``, ``BUILD_DATAIO``.  One should appear
       per project in the current workspace.
     - ON
   * - .. attribute:: USE_ROOT
     - Build with `ROOT <http://root.cern.ch>`_ support. Disabling this option will cause some projects to be skipped when building. IceTop users will want this option to be on.
     - ON
   * - .. attribute:: USE_CINT
     - Build dictionaries with `rootcint <https://root.cern.ch/root/RootCintMan.html>`_. rootcint support is deprecated as of April 1st, 2015, and this option will soon do nothing.
     - OFF
   * - .. attribute:: USE_CCACHE
     - Use `ccache <https://ccache.samba.org/>`_ to aid in compilation. Requires administrator setup in order to use efficiently on multi-user systems.
     - OFF
   * - .. attribute:: CMAKE_BUILD_TYPE
     - Set the cmake build type. Choices are: *None Debug Release RelWithAssert RelWithDebInfo MinSizeRel Coverage*.
     - *Release* or *RelWithAssert*
   * - .. attribute:: SYSTEM_PACKAGES
     - Use your operating system's installed versions of supporting libraries. Safe to use on modern (2015 and newer) OS's, but some more obscure libraries may need to be installed by hand.
     - OFF

Uncommon CMake settings
~~~~~~~~~~~~~~~~~~~~~~~

.. list-table:: Less common configuration variables 
   :header-rows: 1
   
   * - Option
     - Description
     - Default
   * - .. attribute:: ROOT_VERSION
     - Override ROOT version detection if you want to use a specific version of ROOT.
     - autodetected
   * - .. attribute:: GENIE_VERSION
     - Override the Genie version detection if you want to use a specific version of Genie.
     - autodetected
   * - .. attribute:: GENIE_PATH
     - Override the Genie path detection of if you want to use a specific version of Genie.
     - autodetected
   * - .. attribute:: SPHINX_EXTRA_OPTIONS
     - A semicolon delimited list of options to pass to `sphinx <http://sphinx-doc.org/>`_. Useful for developers and documentation writers.
     - "-N;-a;-E"
   * - .. attribute:: USE_DISTCC
     - Use the `distcc <https://github.com/distcc/distcc>`_ distributed compilation system.
     - OFF
   * - .. attribute:: USE_GFILT
     - Use gfilt STL error decryptor. gfilt makes sense of the often long and cryptic C++ compilation errors. However, if you get an unusually confusing compilation error, try building with this option off.
     - ON (under most conditions)
   * - .. attribute:: CHECK_FLAGS
     - Check **some** of the flags passed to the compilers. This option is mostly of interest to developers when adding new compilation/linking flags.
     - OFF
   * - .. attribute:: BUILD_PYBINDINGS
     - Build python bindings. Users will never want to turn this option off, as effective use of the IceCube software stack requires the use of pybindings.
     - ON
   * - .. attribute:: USE_SVN_REVISION_FLAGS
     - Add compiled-in svn revision information. Do not disable this option unless you want to have a bad time.
     - ON
   * - .. attribute:: INSTALL_HEADERS
     - Install header files when making tarball.
     - OFF
   * - .. attribute:: INSTALL_TOOL_LIBS
     - Install libraries from I3_PORTS when making tarball.
     - ON

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

Projects can have compiled test executables and Python tests. The
compiled tests are created with the command::

  make test-bins

The Python tests do not need building.

Tests are run with::

  make test

Beware: The tests need to be run inside ``env-shell.sh``, or they won't work.

If you do ``make test`` in the build directory of your meta-project,
it will run all tests of all projects in the meta-project. That is
usually not what you want, as it takes a long time to complete. 
To run only the tests for a particular project, ``cd`` into its
subdirectory and run ``make test`` there.

The testing system is `CTest <http://www.cmake.org/cmake/help/v2.8.8/ctest.html>`_.
If you want to run only a specific test or more fine-grained control,
you can call ``ctest`` directly, like so::

  ctest -R <regex>

where <regex> is a regular expression which matched against the test labels.
For example, ``ctest -R dataio.*.py`` will run the only the python tests
of dataio.

To see output from the tests for debugging, use option ``-V``.

Information on how to write C++ tests with I3Tests can be found here :doc:`i3test`

.. [wiki:CMakeAddProject   Adding a project]

.. [wiki:CMakeReference   CMakeLists.txt Reference]

