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

You'll need a compiler in order to build the IceCube software. Apple distributes both
clang and llvm-gcc as part of Xcode.

Xcode 4: Lion (10.7) and later
..............................
1) Download Xcode from the App Store.
2) Open Xcode, and open the Preferences window (Xcode > Preferences).
3) In the :ref:`Xcode4Prefs`, click the Downloads tab, and install the Command Line Tools.

.. _Xcode4Prefs:

.. figure:: figs/xcode4_prefs.png
	:width: 432px
	
	Xcode Preferences window

4) After the install finishes you should have both clang and llvm-gcc:

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

MacPorts
""""""""

Do Not Use MacPorts
...................

MacPorts (and Fink, and Homebrew, etc) can cause all sorts of
mismatched library issues. The IceCube software is designed for and
tested against Apple OS X systems without 3rd party package
managers. They cause major headaches and (when it comes to IceCube)
are more trouble than they are worth.

But I Want To Pull Out My Hair, Get An Ulcer, Break My Chair And Possibly Kick A Puppy
......................................................................................

If you do use a 3rd party package manager, and something breaks, you
will be directed to the above paragaph and told to remove it. That
being said, if you want to continue, then take caution.  After you
have "source env.sh", your $PATH variable is updated so that
$I3_PORTS/bin is first. So:

* Always explicitly run $I3_PORTS/bin/port (never let it come from
  your path).

* Never "su" or "sudo" from an xterm where you have "source env.sh",
  as this altered path might be kept and cause conflicts when you
  think you are running a "system" port command.  Start a new xterm
  instead.

* A system Macports install for things like subversion is not needed
  in 10.5/Leopard, since subversion is now supplied by the OS.

Java
""""

As of February  8, 2011, the offline-software meta-project does not
use Java, and related warnings can be safely ignored.

However, the simulation meta-project requires Java. In order for
JAVA_HOME to be set correctly, export using the following command
(bash/sh version)::

  export JAVA_HOME=/Library/Java/Home

which you can set in your ``.bashrc``.

Additionally, as of *Java for Mac OS X 10.6 Update 3 and 10.5 Update
8* Apple no longer ships the Java Native Interface (JNI) headers and
libraries as part of its Java package. The JNI files are included in a
separate Developer package. This package can be installed by doing the
following:

1) Go to http://connect.apple.com/
2) Sign with or create an "Apple Developer ID"
3) Once signed in, in the "Download" sidebar to the right, click "Java"
4) Select the appropriate developer package that matches your OS and Java Update versions.
5) Install the .dmg/.pkg as normal

If you still have problems detecting Java/JNI correctly, try replacing
your ``cmake/tools/jni.cmake`` file with the most current version,
which can always be found here_.

.. _here: http://code.icecube.wisc.edu/icetray/projects/cmake/trunk/tools/jni.cmake

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
"""""""

Offline software is supported on FreeBSD 8 and newer. Versions of ROOT
earlier than 5.30 will not build on 64-bit FreeBSD hosts, so you may need
to pass -DUSE_ROOT=OFF to cmake while building.

