.. SPDX-FileCopyrightText: 2024 The IceTray Contributors
..
.. SPDX-License-Identifier: BSD-2-Clause

.. index::
   single: install
   single: make targets ; install

Installing IceTray
==================

IceTray can simply be used from inside its build directory, but it can also be
installed to a distinct location (the installation prefix), making it
independent of the original source and build directories.

Setting the Installation Prefix
-------------------------------

.. _CMAKE_INSTALL_PREFIX:
.. index:: CMAKE_INSTALL_PREFIX

The installation prefix is set by assigning a value to the
``CMAKE_INSTALL_PREFIX`` CMake variable. Setting it explicitly may also not be
required, as it has some default behaviors:

* The default value (on Unix systems) is ``/usr/local``

* If no other value is set and a Python virtual environment is active when
  CMake is run, it will be set to ``$VIRTUAL_ENV``

Other Installation Options
--------------------------

.. _INSTALL_HEADERS:
.. index:: INSTALL_HEADERS

The ``INSTALL_HEADERS`` option can be used to control whether the header files
are installed. It defaults to being ON; turning it off can save some disk space
(of order ten megabytes) but prevents external, compiled code from being able 
to use the installed copy of IceTray.

.. _LEGACY_INSTALL:
.. index:: LEGACY_INSTALL

The ``LEGACY_INSTALL`` option can be set to cause IceTray to install in a
non-standard manner which it has used in the past. This form of installation
is messy, and may not play well with other software. Effects include:

* ``env-shell.sh`` is placed directly in the installation prefix directory.

* A directory for each project is created directly underneath the installation
  prefix, as in the CMake build directory.

* Header files will not be installed, by default.

* If header files are installed, the header directory for each project will be
  placed directly in the ``include`` directory in the installation prefix,
  instead of being gathered together in an overall ``icetray`` directory, and
  certain 'tool patch' headers will likewise be left loose in the ``include``
  directory.

Installing
----------

.. index::
   single: make install
   single: make targets ; install

Installation is accomplished by running ``make install`` after compiling IceTray.

Using Installed IceTray
=======================

Bare Install
------------

Similar to how ``env-shell.sh`` can be used in the build directory to configure
the environment, when installed IceTray creates a similar script,
``icetray-shell``, which is placed in ``${CMAKE_INSTALL_PREFIX}/bin``. Running
this script creates a subshell in the usual way, such that IceTray tools will
be found in the shell search path, Python modules can be imported, and so on.

For example, if IceTray is installed to the default location of ``/usr/local``,
one can run ``/usr/local/bin/icetray-shell``, which should produce output
similar to the following:

::

   ************************************************************************
   *                                                                      *
   *                   W E L C O M E  to  I C E T R A Y                   *
   *                                                                      *
   *                    Version .main     git:c370e053                    *
   *                                                                      *
   *                You are welcome to visit our Web site                 *
   *                        http://icecube.umd.edu                        *
   *                                                                      *
   ************************************************************************
   
   Icetray environment has:
      I3_SRC       = /usr/local/share/icetray
      I3_BUILD     = /usr/local/share/icetray
      I3_TESTDATA  = /data/icetray/test
      I3_PRODDATA  = /data/icetray/prod
      Python       = 3.12.0

If should be noted that the values set for ``I3_SRC`` and ``I3_BUILD`` will no
longer correspond to the workspace where iceTray was built; this is useful
because that workspace can be removed without affecting the functioning of the
installed copy of the software.

Installation to a Python Virtual Environment
--------------------------------------------

If IceTray is installed to a python virtual environment (via automatic
detection when running CMake), it can be treated exactly like a 'bare'
installation to any other location, e.g. by using the installed
``icetray-shell``. This can also generally be avoided however, as activating
the virtual environment will also add the ``bin`` directory to the shell
executable search path, and the relevant python module directory to the Python
module search path. Furthermore, importing any IceTray Python module will
manipulate the ``I3_SRC``, ``I3_BUILD``, etc. environment variables (for that
process) in a manner equivalent to ``icetray-shell``, such that scripts which
use those variables to locate data files should work normally.
