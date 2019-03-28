Advanced CMake
--------------

.. highlight:: sh 

Editing the CMake cache
"""""""""""""""""""""""

Having run ``cmake`` and generated a ''build'' directory full of
makefiles, one sets build options by editing the CMake cache.  You can
start the editor with "make edit_cache", or by invoking the cache
editor, ``ccmake``, directly::

  cd ~/Icecube/meta-projects/combo/stable/build
  make edit_cache

A handy curses app will pop up allowing you to change the values of
various configuration variables.  As you move the cursor over them a
description is shown at the bottom of the window::

  BOOST_FOUND                      ON                            
  BOOST_INCLUDE_DIR                /Users/troy/ports/include/boos
  BOOST_LIBRARIES                  /Users/troy/ports/lib/boost-1.
  BUILD_DATACLASSES                ON                            
  BUILD_DATAIO                     ON                            

  BOOST_FOUND: BOOST found successfully                                
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

.. _common_cmake_settings:

Common CMake Settings
"""""""""""""""""""""

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
"""""""""""""""""""""""

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
     - Install 3rd party libraries when making tarball.
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


.. [wiki:CMakeAddProject   Adding a project]

.. [wiki:CMakeReference   CMakeLists.txt Reference]

