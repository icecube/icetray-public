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


Karmic Koala
""""""""""""

Support is in as of Nov 20, 2009 for Karmic Koala, with a caveat: The
current ``i3-tools-v3`` 'metatool' requires ``root_5.20.00``... but
that version of root won't build on karmic.  However ``root_5.24.00b``
*will*.  So install ``i3-tools-v3-karmic``, which uses this updated
root, instead.
  
If you are using metaprojects which are already released as of Nov 20
2009 (e.g. those numbered less than 09-11-00) you will also need to
modify the root detection in the build system; replace
``$I3_SRC/cmake/tools/root.cmake`` with a more recent version that
understands ``5.24.00b``, that is, one that contains the following
code::

    elseif(IS_DIRECTORY ${I3_PORTS}/root-v5.24.00b)
      set(ROOT_VERSION "5.24.00b")

