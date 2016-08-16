Getting Started - A guided tour
===============================

Welcome new user! This page should help you get up to speed and get a
working installation of the core Offline-software installed, compiled
and running on your system.  Comments, questions, complaints are
welcome: software(at)icecube.wisc.edu

This process involves:

* Checking various prerequisites on your machine and verifying that
  they work
* Obtaining the external tools that the code requires to build
* Checking out some code
* Building the software
* Running the software

Prerequisites
-------------

While several operating system are supported, there are a few minimum
requirements needed to install, compile and run the core
Offline-software suite.

Operating System
^^^^^^^^^^^^^^^^

Most modern Linux distributions (i386 and x86_64) are supported, as
well as recent Mac OS X (10.10 and newer) and FreeBSD, with one of the following
compilers:

* gcc version 4.4.1 or higher
* clang version 3.6 or higher
* icc version 12 or higher (infrequently tested)

We actively test on a `number of platforms
<http://builds.icecube.wisc.edu/buildslaves>`_.  **We
highly recommend Ubuntu, it contains many useful packages, it's free,
well-supported and has a wide user-base in IceCube.** If you choose
Ubuntu and have problems, please ask questions on the mailing lists.
We won't know about installation problems if you don't point them
out...  and we don't want there to be any installation problems.

Check :ref:`platforms` for the operating system that you are
using. If provided, download and run the script for your operating
system. These scripts will ensure that you have the necessary system
packages installed.

CVMFS
^^^^^

If you are running on a cluster, :ref:`cvmfs` is available to 
manage software installation for you. It provides all of
the standard software used in IceCube.

Checking out some code
----------------------

Now you are ready to check out a release of offline software.  Please
feel free to replace the version used below with the latest version
found in the release directory in svn http://code.icecube.wisc.edu/svn/meta-projects/combo/stable

Check out a release into a workspace
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Generally, it is recommended that you make a single directory as your
workspace make separate subdirectories for source code and another to
hold the resulting "built" libraries and executables.  This will keep
the source code from the code repository separate from the files
created during the build process.

Execute::

 > mkdir ~/combo
 > cd ~/combo
 > svn co $SVN/meta-projects/combo/stable src
    (...produces lots of output...)
 > mkdir build 

You now have a directory filed with::

 build/    src/

Your build/ directory is empty, while your src/ directory contains the
source for all projects in the IceTray release::

 > ls src/
 CMakeLists.txt
 dataclasses/
 icetray/
 dataio/ 
 ...

You are now ready to build!

Build the Software
------------------

Building the software is broken up into two parts:

* Generating local Makefiles using the 'cmake' program (see :ref:`cmake`)
* Building libraries and binary executables with make.

Running cmake
^^^^^^^^^^^^^

We use cmake to build the software::

 > cd ~/combo/build
 > cmake ../src

This will populate your local build directory with directories and
local build files::

 -- Check for working C compiler: /usr/bin/gcc
 -- Check for working C compiler: /usr/bin/gcc -- works
 -- Check size of void*
 -- Check size of void* - done
 -- Check for working CXX compiler: /usr/bin/c++
 -- Check for working CXX compiler: /usr/bin/c++ -- works
 -- I3_PORTS     /icework/blaufuss/i3tools
 -- OSTYPE       Linux
 -- OSVERSION    2.6.22-14-generic
 -- ARCH         i686
 -- BUILDNAME    Linux-2.6.22-14-generic/i686/gcc-4.4.3
                     ....
 --   interfaces
 --   coordinate-service
 --   examples
 -- Generating env-shell.sh
 -- Generating 
 -- Generating tarball_hook.sh
 -- Configuring 'gfilt' STL decryptor
 -- Configuring done
 -- Generating done
 -- Build files have been written to: /home/blaufuss/combo/build

You're ready to build.

Build it!
^^^^^^^^^

In your ~/combo/build directory execute::

 make

You will see the build progress::

 [  0%] Generating /disk02/home/blaufuss/icework/combo/build/icetray/icetrayDict.cxx with rootcint
 Scanning dependencies of target icetray
 [  1%] Building CXX object icetray/CMakeFiles/icetray.dir/private/icetray/I3Frame.o
 [  1%] Building CXX object icetray/CMakeFiles/icetray.dir/private/icetray/open.o
 [  1%] Building CXX object icetray/CMakeFiles/icetray.dir/private/icetray/load_project.o
                      ....
 [102%] Building CXX object examples/CMakeFiles/examples.dir/private/modules/HelloWorld.o
 [102%] Building CXX object examples/CMakeFiles/examples.dir/private/modules/Smear.o
 [102%] Building CXX object examples/CMakeFiles/examples.dir/private/services/FibonacciServiceFactories.o
 [102%] Building CXX object examples/CMakeFiles/examples.dir/examplesDict.o
 Linking CXX shared library ../lib/libexamples.so
 [102%] Built target examples

Cmake nicely displays a fraction complete so you can follow the build
to completion.

Rsync the test-data
^^^^^^^^^^^^^^^^^^^

If you're not using a CVMFS toolset on a cluster, sync the test-data
to your local test-data directory ($I3_TESTDATA)::

 make rsync

This will download >1GB of data used by testing and example
scripts.  If you already have them, this command will simply make sure
you copy of test-data is up to date.

Using the software
------------------

Once compiled, you can explore some of the provided example scripts.
Each project typically has several example This is a simple tour.

Loading the environment
^^^^^^^^^^^^^^^^^^^^^^^

This part is straightforward. Assuming that you are starting from a
fresh shell (one that contains no information about your any IceCube
workspace), you should read one of these files into your
workspace. Use the::

 > ./env-shell.sh
 
which again should produce output roughly like this::

 ************************************************************************
 *                                                                      *
 *                   W E L C O M E  to  I C E T R A Y                   *
 *                                                                      *
 *                Version combo.trunk     r150911                       *
 *                                                                      *
 *                You are welcome to visit our Web site                 *
 *                        http://icecube.umd.edu                        *
 *                                                                      *
 ************************************************************************
 
 Icetray environment has:
    I3_SRC       = /home/blaufuss/combo/src
    I3_BUILD     = /home/blaufuss/combo/build

This has setup up your PATH, LD_LIBRARY_PATH and other environment
variables so that you are now ready to run IceTray python scripts and
executables.  This file should work equally well for bash-like and
csh-like shells.

A few standard enviroment variables are also set (and often referred
to in scripts, code, etc):

* I3_SRC - Pointer to your local src area, where you checked out the
  source code from $SVN
* I3_BUILD - Pointer to your local build area, where you build IceTray
  libraries and executeables.

If you load your environment twice, you'll be warned::

 % ./env-shell.sh
 ****************************************************************
 You are currently in a shell with an Icetray environment loaded.
 Please exit the current shell and re-run ./env-shell.sh from a clean shell.
 ****************************************************************
 Environment not (re)loaded.

This is not a fatal situation and your PATH and LD_LIBRARY_PATH have
not modified again. Still there are probably some ways to get into
trouble (if your toolset has changed since the last time you loaded
your environment, and you try to run a binary...). You are still
better off starting a new shell before you reload these scripts.

