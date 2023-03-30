.. _supercomputing_centers:

IceCube's ``icetray`` at "supercomputing centers"
=================================================

The following is a tutorial for getting stared with ``icetray`` at
"supercomputing centers". Wile originally written for `TACC
<https://www.tacc.utexas.edu/>`_, it should generally apply to
supercomputing or large-scale time-sharing centers.

----

This is a basic example of configuring ``icetray`` @ TACC. Not all
dependencies are covered, but with this guide users should be able to
configure any remaining dependencies. This guide also assumes a working
familiarity with UNIX shells and TACC's systems specifically
``frontera``.

First steps
-----------

Either by hand, or in your shell init files load the following modules:

.. code:: bash

    module load boost
    module load cuda
    module load fftw3
    module load gsl
    module load suitesparse

Then set the following environment variables. If adding them to your
shell init files, be sure to "restart" your shell:

.. code:: bash

    export I3_SITE_CMAKE_DIR=${HOME}/i3/site-cmake
    export CUDA_DIR=${TACC_CUDA_DIR}
    export CC=icc
    export CXX=icpc

.. warning::

   The above variables, CC and CXX, should be set to your chosen
   compiler, otherwise you may get configuration or build errors.

Create the ``site-cmake`` directory,

.. code:: console

    $ mkdir ~/i3/site-cmake

and then copy the contents of this directory to it.

Finally checkout ``icetray`` from GitHub.

Using dependencies provided by TACC
-----------------------------------

When configuring ``icetray`` by running CMake, dependencies may not be
picked up even when they are available @ TACC. If this is the case for
you, first ensure that you've loaded the relevant modules by checking
``module list``, and if necessary loading it with
``module load <module name>``.

If the module is loaded, you may need to create an override for that
dependency. This is done by adding file to what is generically termed a
"site cmake directory". By default, this directory is
``/usr/share/fizzicks``. (Yes, a silly name, but chosen at the time to
avoid conficts.) This can be overridden by setting the environment
variable ``I3_SITE_CMAKE_DIR`` in your shell.

The files in your site-cmake directory should mirror those in
``cmake/tools/`` of the ``icetray`` source. Start by copying the
relevant tool detection file to your site-cmake directory. If the file
uses the ``tooldef()`` macro this is generally what you should use too.
A good example of this is the ``fftw3.cmake`` file included in this
example site-cmake.

The original version looks like:

.. code:: cmake

    #
    #  cmake/tools/fftw3.cmake
    #

    tooldef(fftw3
       include
       fftw3.h
       lib
       NONE # bin is n/a, placeholder
       fftw3
       )

To configure this tool at TACC we use the following in our example
site-cmake:

.. code:: cmake

    ## fftw3 detection for fftw3
    ## make sure you've loaded the fftw3 module

    tooldef(fftw3
       $ENV{TACC_FFTW3_INC}
       fftw3.h
       $ENV{TACC_FFTW3_LIB}
       NONE # bin is n/a, placeholder
       fftw3
       )

The signature of the ``tooldef()`` macro is:

.. code:: cmake

    tooldef(TOOL_NAME
      INCLUDE_DIRECTORY_HINT
      INCLUDE_FILE
      LIBRARY_DIRECTORY_HINT
      BINARY_DIRECTORY_HINT
      LIBRARY_NAME_1
      LIBRARY_NAME_2
      ...
    )

Installing dependencies not provided by TACC
--------------------------------------------

Unfortuantelly TACC doesn't include all of ``icetray``'s dependencies.
For these the user will have to install them by hand. For the purposes
of this guide, the author installed each dependency into its own
directory under ``${WORK}/i3``. For example CFITSIO was configured with
``./configure --prefix=${WORK}/i3/cfitsio ...`` and then installed via
``make install``. This layout generally works well, and eases
maintenance.

Using the ``tooldef()`` macro in this case isn't much different. For
example, for CFITSIO we set the ``cfitsio.cmake`` file to:

.. code:: cmake

    ## cfitsio detection @ TACC

    tooldef(cfitsio
        $ENV{WORK}/i3/cfitsio/include
        fitsio.h
        $ENV{WORK}/i3/cfitsio/lib
        NONE
        cfitsio
    )

But what if I can't use the ``tooldef()`` macro
-----------------------------------------------

If you can't (or don't want to) use the ``tooldef()`` macro, that's
fine. You *do* need to ensure that at least the following CMake
variables are set in your override file:

-  ``xxx_INCLUDE_DIR``
-  ``xxx_LIBRARIES``
-  ``xxx_FOUND``

Where "xxx" represents the tool name in all uppercase. (CMake variable
names are case sensitive.) Additionally, ``xxx_LIBRARIES`` is a CMake
list (semi-colon delimited) of full paths to the library files. You can
use CMake's ``set()`` and ``list()`` commands directly, or combinations
of ``find_package()``, ``find_path()`` and ``find_library()``. If using
CMake's ``find_package()``, check that module's documentation. The
variables it sets may need to be assigned to the variables ``icetray``
expects. An example of this is in the ``opencl.cmake`` file
included here.

But what about linuxbrew / Homebrew-on-Linux?
---------------------------------------------

Homebrew is used for ``icetray`` on macOS. In regards to
Homebrew-on-Linux, assuming you've installed it properly, and have the
``brew`` command in your path, like on macOS, running
``brew install -f Brewfile`` from the root of ``icetray`` source
directory will install all the necessary dependencies for you. Then,
``icetray`` will pick those dependencies automatically.

.. warning::

   We are not endorsing installing Homebrew-on-Linux at TACC.  Check
   TACC's policies before doing so. At the very least, Homebrew has
   the potential to eat into your filesystem quota.

What about Python support?
--------------------------

Use your chosen virtual env, and its module install method to install
the dependencies in ``requirements.txt``. Be sure to activate that
virtual env before configuring (and using!) ``icetray``.

See also...
-----------

- :ref:`tooldetection` -- ``cmake/resources/docs/toolset.rst`` in the ``icetray`` source.
- ``cmake/tools/tooldef.cmake`` in the ``icetray`` source.
- ``cmake/tools/*.cmake`` (the various tool detection files) in the
  ``icetray`` source.

fini
~~~~
