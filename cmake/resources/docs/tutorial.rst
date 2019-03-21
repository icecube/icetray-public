User's Guide
------------

.. highlight:: sh 

`CMake <http://www.cmake.org/>`_ is a cross-platform build system which
generates build files for use by native build systems for different
platforms.  On UNIX-like systems it generates makefiles.  CMake provides 
easier per-project build configuration, a more standard build
environment across different platforms, additional testing infrastructure, 
and out-of-source compilation.

Building with cmake
^^^^^^^^^^^^^^^^^^^

To begin building with CMake start with a clean workspace. We will assume
an environment from :ref:`cvmfs` or :ref:`platforms`  is already set up and loaded.

With CMake, one creates "out-of-source" builds.  This makes it easier to
build several configurations against one set of source, and to be sure
that the build process does not modify the source itself (e.g. no
temporary files end up laying around in your checkout).  So create a
work directory and check out source to a directory called ``src``:

::

  mkdir -p ~/IceCube/meta-projects/combo/stable
  cd ~/IceCube/meta-projects/combo/stable
  svn co http://code.icecube.wisc.edu/svn/meta-projects/combo/stable src

.. note::

   The "src" on the end of the ``svn co`` command tells it to check out
   to the "src" directory. You may choose any directory you like.   

Create the build directory parallel to the source directory and
execute ``cmake`` in the object directory to create the build environment::

  $ pwd
  ~/IceCube/meta-projects/combo/stable
  $ mkdir build
  $ ls
  build src
  $ cd build
  $ cmake ../src
  <lots of cmake output omitted>
  -- Configuring done
  -- Generating done
  -- Build files have been written to: ~/IceCube/meta-projects/combo/stable/build

At this point the contents of the object directory should look
somewhat like that of the source directory.  Main differences:

* there are no sourcefiles in the subdirectories of ``build/``
* build contains directories ``CMakeFiles/``, ``bin/``, and ``lib/``
* build contains files ``CMakeCache.txt``, ``env-shell.sh``, 
  and ``tarball_hook.sh``.

While still in the build directory execute ``make``::

  $ pwd
  ~/IceCube/meta-projects/combo/stable/build
  $ make

After ``make`` returns, your build results will be in the
build directory.  Assuming no builds are made inside of the original
source directory that directory can be reused for other builds, even if
the platforms or build configurations differ.

The ``make`` target ``help`` will provide a list of
valid targets in the CMake-generated ``Makefile``::

  $ pwd
  ~/IceCube/meta-projects/combo/stable/build
  $ make help
  The following are some of the valid targets for this Makefile:
  ... all (the default if no target is provided)
  ... clean
  <more targets omitted>

.. _rebuild_cache: 

Command line configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^

If you know what you want to set, you can do so from the
command line.  For instance, I have checked out source to directory
``src/`` and am in a parallel directory ``build/``.  I want to build
the "release" variant, the one with optimizations on, i issue::

   cmake -DCMAKE_BUILD_TYPE=Release ../src

See :ref:`common_cmake_settings` for details.

The cmake cache CMakeCache.txt
""""""""""""""""""""""""""""""

Cache variables are stored in a file ``CMakeCache.txt`` in the
build directory. If you are trying to adjust software dependencies,
or doing anything strange with the build, try deleting this file
to clear the cache.

.. warning::

  **If you add or remove projects** you must run ``make
  rebuild_cache``.  This is because the list of projects in the
  workspace is collected with a glob.

.. _testdriver:

Testing
^^^^^^^

Projects can have compiled test executables and Python tests. The
compiled tests are created with the command::

  make test-bins

The Python tests do not need building.

Tests are run with::

  make rsync
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
