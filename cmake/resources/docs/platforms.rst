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

IceTray requires some additional support software to build and function. Before
you gallop off and try to install all of the dependencies by hand, please read
these notes in their entirety. Especially:

.. warning:: Do not try to install your own Python over the perfectly good version shipped with the base system. It is very likely to end in tears. This includes the Enthought and Anaconda distributions as well as the Python formula in homebrew; they do not play nicely with IceTray. See :ref:`osxpythonsetup` for more information.

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

3) In the :ref:`Xcode4Prefs`, click the Downloads tab, and install the
   Command Line Tools.

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
should also `tap`_ homebrew/science and IceCube-SPNO/icecube. Install them like
this::

	brew install cmake

The following formulae are recommended:

* offline-software: boost boost-python cmake cdk gsl hdf5 libarchive mysql qt pyqt 
* IceRec: cfitsio minuit2 suite-sparse
* simulation: sprng2

.. warning:: Some Homebrew formulas have Python as a dependency, so a second Python may sneak onto your computer without your knowledge. To avoid this, install formulas that depend on python with the parameter ``--build-from-source``. See `Homebrew's notes on Python`_ for further information.

.. _tap: https://github.com/Homebrew/homebrew/wiki/brew-tap
.. _`Homebrew's notes on Python`: https://github.com/Homebrew/homebrew/blob/master/share/doc/homebrew/Homebrew-and-Python.md#why-is-homebrews-python-being-installed-as-a-dependency

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


.. _osxpythonsetup:

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

among other things, this creates a script
:command:`~/.virtualenv/standard/bin/active` that can be used to set up
the environment. I put these lines in my .bash_login/.zlogin script to 
enter this one automatically whenever I start a new shell::
	
	VIRTUAL_ENV_DISABLE_PROMPT=1
	. ~/.virtualenv/standard/bin/activate

Inside the environment, :command:`pip` will automatically install packages in
the environment rather than in /Library/Python. Now you can install
bleeding-edge versions of Python packages to your heart's content::
	
	pip install numpy
	pip install matplotlib
	pip install ipython

Pitfalls
........

::
	
	clang: error: unknown argument: '-mno-fused-madd' [-Wunused-command-line-argument-hard-error-in-future]

The version of clang distributed with XCode 5.1 deprecates some GCC-only flags.
Whereas they had previously been silently ignored, they now raise errors when
used. For the time being you can work around this change by setting an
environment variable::
	
	export ARCHFLAGS=-Wno-error=unused-command-line-argument-hard-error-in-future

.. index:: RHEL4
.. _RHEL4:

Red Hat Variants
^^^^^^^^^^^^^^^^

For Red Hat variants it is best to use I3_PORTS to install dependent software.

Red Hat
"""""""

* Red Hat Enterprise Linux 4, and its derivitives, may not work with
  IceCube software as they fall outside of the "current/previous" rule
  of thumb stated above.

* TCL dependencies are satisfied only with the tcl-devel package. Try
  'up2date tcl-devel' as root user.

.. index:: Scientific Linux

Scientific Linux
""""""""""""""""

There is a script at
http://code.icecube.wisc.edu/icetray-dist/distros/ScientificSL.sh
that will install the necessary packages.

After installing these packages, run "easy_install -U Sphinx" to install
Python Sphinx for using the documentation system. 

.. index:: CentOS
.. _centos:

CentOS
""""""

There is a script at
http://code.icecube.wisc.edu/icetray-dist/distros/CentOS.sh
that will install the necessary packages.


After installing these packages, run "easy_install -U Sphinx" to install
Python Sphinx for using the documentation system. 

Fedora
""""""

There is a script at
http://code.icecube.wisc.edu/icetray-dist/distros/Fedora.sh
that will install the necessary packages.

After installing these packages, run "easy_install -U Sphinx" to install
Python Sphinx for using the documentation system. 

.. index:: Ubuntu
.. index:: Debian

.. _ubuntu:
.. _debian:

Ubuntu or Debian Variants
^^^^^^^^^^^^^^^^^^^^^^^^^

Using the Package Manager
"""""""""""""""""""""""""

On laptops or user-controlled workstations it is recommended to install
as much as possible through the package manager.

Basic Packages (offline-software)
.................................

For a basic system (such as viewing files, basic tasks), install:

.. container:: wrapped-code

    apt-get install build-essential cmake libbz2-dev libgl1-mesa-dev 
    freeglut3-dev libxml2-dev subversion libboost-python-dev 
    libboost-system-dev libboost-signals-dev libboost-thread-dev 
    libboost-date-time-dev libboost-serialization-dev libboost-filesystem-dev 
    libboost-program-options-dev libboost-regex-dev libboost-iostreams-dev 
    libgsl0-dev libcdk5-dev libarchive-dev python-scipy ipython-qtconsole 
    libqt4-dev python-urwid

For newer Ubuntu versions (>=14.04):

.. container:: wrapped-code

    apt-get install libz-dev libqt4-opengl-dev

Simulation Packages
...................

Simulation doesn't require too much extra. cfitsio is required, the rest
is optional depending on your use case:

.. container:: wrapped-code

    apt-get install libcfitsio3-dev libsprng2-dev libmysqlclient-dev
    libsuitesparse-dev

IceRec Packages
...............

Some packages are easily available:

.. container:: wrapped-code

    apt-get install libcfitsio3-dev libmysqlclient-dev libhdf5-serial-dev

Root is only available in newer releases (Ubuntu 13.10+, Debian 7+):

.. container:: wrapped-code

    apt-get install root-system

Other Tools
...........

If you want Geant4 or Genie, you'll need to use I3_PORTS.

Using I3_PORTS
""""""""""""""

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

The following packages are recommended.

offline-software
""""""""""""""""

Install this to get the basics:

.. container:: wrapped-code

    pkg install bash subversion cmake boost-libs boost-python-libs cdk gsl hdf5 ccache

IceRec
""""""

Install this in addition to the offline-software packages to be able to run
reconstructions:

.. container:: wrapped-code

    pkg install cfitsio gotoblas suitesparse py27-numpy

.. Note::
    
    suitesparse will automatically install the non-optimized netlib
    BLAS/LAPACK for you. If you have gotoblas installed, though, CMake will
    link against it as expected.
