.. _platforms:

Details by Platform
===================

.. index:: OSX
.. _OSX:

Apple Mac OS X
^^^^^^^^^^^^^^

* If you use MacPorts to manage your "extra" system tools, like
  subversion, then take caution.  After you have "source env.sh", your
  $PATH variable is updated so that $I3_PORTS/bin is first. So:

  * Always explicitly run $I3_PORTS/bin/port (never let it come from
    your path).

  * Never "su" or "sudo" from an xterm where you have "source env.sh",
    as this altered path might be kept and cause conflicts when you
    think you are running a "system" port command.  Start a new xterm
    instead.

  * A system Macports install for things like subversion is not needed
    in 10.5/Leopard, since subversion is now supplied by the OS.


.. index:: RHEL4
.. _RHEL4:

RHEL4
^^^^^

* TCL dependencies are satisfied only with the tcl-devel package. Try
  'up2date tcl-devel' as root user.

.. index:: Fedora Core
.. _fedoracore:

Fedora Core
^^^^^^^^^^^

* TCL dependencies are satisfied only with the tcl-devel package. Try
  'yum install tcl-devel'.

Fedora Core 9: The following packages were needed after a fresh
install of Fedora Core 9:

* curl
* tcl
* g77

.. index:: Ubuntu
.. index:: Debian

.. _ubuntu:
.. _debian:

Ubuntu 8.04 (and other Debian/Debian-based OS's)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Since Ubuntu Linux comes by default on just a single CD, you will need
to install several packages before installing the IceCube toolset and
software.  These are pretty standard things, but are generally needed
only for development purposes, so they are not included on the Ubuntu
LiveCD.  The easiest thing to do is use the "Synaptic Package
Manager", which you can start from: System-->Administration.  You need
to install the following, which are not there by default::

  autoconf
  automake
  bzip2
  cmake
  curl
  g++
  g77
  gcc
  libhtml-parser-perl
  libbz2-dev
  libmysqlclient-dev
  libgl-dev 
  libglut-dev
  libpcre3-dev
  libssl-dev
  libx11-dev 
  libxft2-dev
  libxi-dev
  libxmu-dev
  libxpm-dev
  libxt-dev
  m4
  make
  ncurses-dev
  patch
  python-dev
  rsync
  subversion (svn)
  sun-java5-jdk
  tcl8.4-dev
  texinfo

This list may be incomplete, please contact us with updates.

In order for JAVA_HOME to be set correctly, export using the following
command: (If you installed the above version of java using the package
manager)::

 export JAVA_HOME=/usr/lib/jvm/java-5-sun


The following may not be required for 8.04 (Hardy Heron) and/or newer
releases of Ubuntu:

* cernlib-core (some graphics libs included in this package's
  dependencies are needed. otherwise the ROOT TBrowser will crash
  horribly. black magic.)  (just search for these names in the package
  manager)

* In addition, in order for the system to recognize the tcl
  installation, you will also need to do this::

    ln -s /usr/lib/tcl8.4/tclConfig.sh /usr/lib/.

  (this is just a quirk of the i3-install.sh script)

