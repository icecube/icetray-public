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

.. index:: CentOS 5.4 & Scientific Linux 5.4

.. _centos:

CentOS 5.4 and Scientific Linux 5.4
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The installation of these packages requires "root" or "sudo" privileges.
The packages required for both of these OS versions are the same. They need
to be installed using "yum -y install <package name>"

  compat-gcc-34-g77
  doxygen
  expat-devel
  freeglut-devel
  gcc
  gcc-c++
  gcc-gfortran.i386
  libX11-devel
  libXext-devel
  libXfixes-devel
  libXft-devel
  libXi-devel
  libXmu-devel
  libXpm-devel
  libXrandr-devel
  libXt-devel
  ncurses-devel
  openssl-devel
  pcre-devel
  python-devel
  python-setuptools
  subversion
  tcl-devel
  texinfo
 
After installing these packages, run "easy_install -U Sphinx" to install
Python Sphinx for using the documentation system. 


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
  curl
  doxygen
  g++
  g77
  libbz2-dev
  libc6-dev
  libexpat1-dev
  libglut
  libglut-dev
  libhtml-parser-perl
  libpcre3-dev
  libx11-dev
  libxft-dev
  libxi-dev
  libxmu-dev
  libxpm-dev
  libxt-dev
  m4
  ncurses-dev
  openssl-dev
  patch
  python-dev
  python-setuptools
  ssl-dev
  subversion
  sun-java6-jdk
  tcl
  tcl-dev
  texinfo
  xlibmesa-gl
  xlibmesa-gl-dev
  xlibmesa-glu
  xlibmesa-glu-dev

This list may be incomplete, please contact us with updates.

In order for JAVA_HOME to be set correctly, export using the following
command: (If you installed the above version of java using the package
manager)::

 export JAVA_HOME=/usr/lib/jvm/java-6-sun


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

.. index:: Karmic Koala

Ubuntu 9.10 Karmic Koala
^^^^^^^^^^^^^^^^^^^^^^^^
The following packages are required for Karmic Koala::

  autoconf
  curl
  doxygen
  g++
  libexpat1-dev
  libglut
  libglut-dev
  libhtml-parser-perl
  libpcre3-dev
  libx11-dev
  libxft-dev
  libxi-dev
  libxmu-dev
  m4
  ncurses-dev
  openssl-dev
  patch
  python-dev
  python-setuptools
  python-sphinx
  subversion
  sun-java6-jdk
  tcl
  tcl-dev
  texinfo
  xlibmesa-gl
  xlibmesa-gl-dev
  xlibmesa-glu
  xlibmesa-glu-dev

Please note that in some cases the normal user distributions must be installed
before ones tagged with -dev package names. The "svn" command comes with the 
subversion package as a reminder of how it is listed. 

Support is in as of Nov 20, 2009 for Karmic Koala, with a caveat: nThe
current ``i3-tools-v3`` 'metatool' requires ``root_5.20.00``.  It
requires other things as well, you can see them with ``port deps
i3-tools-v3``::

  % /opt/i3/ports/bin/port deps i3-tools-v3
  i3-tools-v3 has library dependencies on:
          gsl_1.8
          rdmc_2.9.5
          log4cplus_1.0.2
          sprng_2.0a
          slalib-c_0.0
          mysql_4.1.20
          photonics_1.67
          root_5.20.00
          boost_1.38.0
          cmake
          anis_1.0
          cdk
          hdf5_1.6.8
          ptd_1.0.2
  
All of that stuff builds with the exception of that version of root.
The root version that will actually build on ubuntu 9.10 is
``5.24.00b``.  So to get things going, individually install each of
the ports in the list above with ``port install`` (e.g. ``port install
photonics_1.67``), except root, which should be ``root_5.24.00b``.  

If you are using metaprojects which are already released as of Nov 20
2009 (e.g. those numbered less than 09-11-00) you will also need to
modify the root detection in the build system; replace
``$I3_SRC/cmake/tools/root.cmake`` with a more recent version that
understands ``5.24.00b``, that is, one that contains the following
code::

    elseif(IS_DIRECTORY ${I3_PORTS}/root-v5.24.00b)
      set(ROOT_VERSION "5.24.00b")

