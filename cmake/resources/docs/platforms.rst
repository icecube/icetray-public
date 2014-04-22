.. _platforms:

Details by Platform
===================

Unlike many other projects / experiments, IceCube strives to be as
Operating System agnostic as possible and maintains no "standard
platform". That being said, manpower and hardware are limited, and all
possible platforms and combinations of platfroms cannot be tested or
supported. A general rule of thumb is that the latest "long term" or
"supported" or "regular life cycle" release, as well as the previous
one, will work with IceCube software. Use common sense and you should
be safe. If you're curious as to what platforms are regularly tested,
take a look at our list_.

Below you can find some hints and tips that we've collected about
various platforms.

.. _list: http://builds.icecube.wisc.edu/

.. index:: OSX
.. _OSX:

Apple Mac OS X
^^^^^^^^^^^^^^

Developer Tools
"""""""""""""""

You'll need a compiler in order to build the IceCube software. Apple distributes
clang and llvm-gcc both as a stand-alone download and as part of its Xcode IDE,
but requires you to register for an `Apple ID`_  before you can download either
one. The Xcode download is 10 times larger than the command-line tools package,
so you should only download it if you need it for non-IceCube work.

.. _`Apple ID`: https://appleid.apple.com

Command-Line Tools
..................
1) Go to the `Apple Developer Downloads page <http://developer.apple.com/downloads>`_,
   signing in with your `Apple ID`_.
2) Download and install the latest Command Line Tools.

Xcode
.....
1) Download Xcode from the App Store.
2) Open Xcode, and open the Preferences window (Xcode > Preferences).
3) In the :ref:`Xcode4Prefs`, click the Downloads tab, and install the Command Line Tools.

.. _Xcode4Prefs:

.. figure:: figs/xcode4_prefs.png
	:width: 432px
	
	Xcode Preferences window

After the install finishes you should have both clang and llvm-gcc:

.. code-block:: sh
	
	bash-3.2$ g++ --version
	i686-apple-darwin11-llvm-g++-4.2 (GCC) 4.2.1 (Based on Apple Inc. build 5658) (LLVM build 2336.11.00)
	Copyright (C) 2007 Free Software Foundation, Inc.
	This is free software; see the source for copying conditions.  There is NO
	warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	bash-3.2$ clang++ --version
	Apple clang version 4.0 (tags/Apple/clang-421.0.60) (based on LLVM 3.1svn)
	Target: x86_64-apple-darwin12.0.0
	Thread model: posix

Package managers
""""""""""""""""

As noted in :ref:`do-i-need-i3ports`, you can set up your environment quite
quickly if you use a package manager. While Apple doesn't provide an official
package manager for OS X, there are a number of 3rd implementations to choose
from.

.. _Homebrew: http://brew.sh
.. _MacPorts: https://www.macports.org
.. _Fink: http://www.finkproject.org

Homebrew
........

Homebrew_ is probably the easiest way to install packages on OS X, and
distributes the most heavy-weight dependencies (cmake, boost, and Qt) as binary
packages. Most of the required formulae are in the main distribution, but you
should also `tap`_ homebrew/science and jvansanten/icecube. Install them like this::
	brew install cmake
The following formulae are recommended:

* offline-software: boost cmake cdk gsl hdf5 libarchive mysql qt pyqt 
* IceRec: cfitsio minuit2 suite-sparse
* simulation: sprng2

.. _tap: https://github.com/Homebrew/homebrew/wiki/brew-tap

.. warning:: Don't install Python from Homebrew. If you do, it will also want to waste a lot of time building boost from source instead of using a binary distribution built against the system Python (2.7.2, which is just fine).

MacPorts
........

I3_PORTS shares a common ancestor with MacPorts_. As a consequence, they both
use :command:`port` but expect to be managed at different privilege levels,
which can lead to all sorts of subtle inconsistencies and outright breakage if
you confuse them. If you can at all avoid it, do not use I3_PORTS and MacPorts_
together. If you must, however, here are some pitfalls to avoid:

* Always explicitly run :command:`$I3_PORTS/bin/port` (never let it come from
  your path).

* Never :command:`su` or :command:`sudo` from a terminal windows where you
  have run :command:`env-shell.sh`, as this altered path might be kept and
  cause conflicts when you think you are running a "system" :command:`port`
  command. Start a new terminal instead.

Fink
....

Does anyone still use Fink_? If you do, and think it's any good, write some
documentation.

.. _osx-python-setup::

Python on OS X
""""""""""""""

Starting with 10.6, Apple has shipped a fairly up-to-date Python interpreter
and standard library with OS X, and there's no good reason to replace it. At
the same time, it has started shipping 3rd-party libraries with more rapid
release cycles like :py:mod:`matplotlib`, :py:mod:`numpy`, and
:py:mod:`ipython` in /Library/Python/X.Y/site-packages. You may wish to upgrade
these packages to take advantage of new features and performance improvements.
The problem is that Apple considers that path part of the OS, and will wipe out
your changes when you update the OS. You can get around this by using
:py:mod:`virtualenv` to use to create an environment that explicitly
overrides the system site-packages and is isolated from other environments.

.. highlight:: sh

First, install :command:`pip`, a better package manager for Python::
	
	sudo easy_install pip

then, use :command:`pip` to install :py:mod:`virtualenv`::
	
	sudo pip install virtualenv

This ends the privileged portion. Now, create a new virtual environment.
I call mine ".virtualenv/standard"::
	
	virtualenv .virtualenv/standard
among other things, this creates a script :command:`~/.virtualenv/standard/bin/active`
that can be used to set up the environment. I put these lines in my .bash_login/.zlogin
script to enter this one automatically whenever I start a new shell::
	
	VIRTUAL_ENV_DISABLE_PROMPT=1
	. ~/.virtualenv/standard/bin/activate
Inside the environment, :command:`pip` will automatically install packages in
the environment rather than in /Library/Python. Now you can install bleeding-edge
versions of Python pacakges to your heart's content::
	
	pip install numpy
	pip install matplotlib
	pip install ipython

.. index:: RHEL4
.. _RHEL4:

Red Hat
^^^^^^^

* Red Hat Enderprise Linux 4, and its derivitives, may not work with
  IceCube software as they fall outside of the "current/previous" rule
  of thumb stated above.

* TCL dependencies are satisfied only with the tcl-devel package. Try
  'up2date tcl-devel' as root user.

.. index:: Scientific Linux

Scientific Linux
^^^^^^^^^^^^^^^^

There is a script at
http://code.icecube.wisc.edu/icetray-dist/distros/ScientificSL.sh
that will install the necessary packages.

After installing these packages, run "easy_install -U Sphinx" to install
Python Sphinx for using the documentation system. 

.. index:: CentOS
.. _centos:

CentOS
^^^^^^

There is a script at
http://code.icecube.wisc.edu/icetray-dist/distros/CentOS.sh
that will install the necessary packages.


After installing these packages, run "easy_install -U Sphinx" to install
Python Sphinx for using the documentation system. 

Fedora
^^^^^^

There is a script at
http://code.icecube.wisc.edu/icetray-dist/distros/Fedora.sh
that will install the necessary packages.

After installing these packages, run "easy_install -U Sphinx" to install
Python Sphinx for using the documentation system. 

.. index:: Ubuntu
.. index:: Debian

.. _ubuntu:
.. _debian:

Ubuntu or Debian
^^^^^^^^^^^^^^^^

There is a script at
http://code.icecube.wisc.edu/icetray-dist/distros/Ubuntu.sh
that will install the necessary packages.

To install further packages (emacs and so forth), the easiest thing to
do is use the "Synaptic Package Manager", which you can start from menu
``System --> Administration``. 

In order for JAVA_HOME to be set correctly, export using the following
command (bash/sh version)::

  export JAVA_HOME=/usr/lib/jvm/java-6-sun

which you can set in your ``.bashrc``.


NB: Its is *highly* recommended to use LTS ( Long-Term Support )
releases of Ubuntu.  There are known issues with Natty Narwhal 
( Ubuntu 11.04 ) and Oneiric Ocelot ( Ubuntu 11.10 ) and if you 
want something newer sit tight for Precise Pangolin ( Ubuntu 12.04 ), 
which is the next LTS release.

FreeBSD
^^^^^^^

Offline software is supported on FreeBSD 8 and newer. Versions of ROOT
earlier than 5.30 will not build on 64-bit FreeBSD hosts, so you may need
to pass -DUSE_ROOT=OFF to cmake while building.

The FreeBSD base system includes a complete compiler toolchain, and the
FreeBSD Ports Collection includes binary packages that can be installed like
so::
	pkg install cmake

The following packages are recommended:

* offline-software: bash subversion cmake boost-libs boost-python-libs cdk gsl hdf5
* IceRec: cfitsio gotoblas suitesparse

.. Note:: suitesparse will automatically install the non-optimized netlib BLAS/LAPACK for you. If you have gotoblas installed, though, CMake will link against it as expected.
