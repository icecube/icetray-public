.. SPDX-FileCopyrightText: 2024 The IceTray Contributors
..
.. SPDX-License-Identifier: BSD-2-Clause

.. _pal-tool:

PAL
===

PAL stands for Positional Astronomy Library. It provides standard routines for
astronomical calculations. The PAL library is a partial re-implementation of
Pat Wallace's popular SLALIB library written in C using a Gnu GPL license and
layered on top of the IAU's SOFA library (or the BSD-licensed ERFA) where
appropriate.  SLALIB is an old Fortran library which was translated into C, it
is no longer updated, has a weird license, and is hard to find.

IceCube's astronomical coordinate software :ref:`astro` requires PAL to work.
The PAL source code is available on `github
<https://github.com/Starlink/pal>`_, but it is difficult to compile. A `fork
<https://github.com/icecube/pal>`_ provided by IceCube which is much
easier to compile is available from the IceCube Github repository, it depends
on a library called ERFA.

Apple Mac OS X
--------------

On MAC OS X, homebrew can be used:

.. code-block:: console

  brew tap icecube/homebrew-icecube
  brew install pal

For other systems, check your package manager. It maybe called ``pal`` or ``starlink-pal``.

Install from Source
--------------------

PAL depends on a library called ERFA which can be downloaded from github.

.. code-block:: console

  git clone https://github.com/liberfa/erfa
  cd erfa
  ./bootstrap.sh
  ./configure
  make
  make install

For Ubuntu you may also have to install **autoconf** and **libtool**, depending on your
specific distribution. :

PAL can be installed by downloading from github and compiling:

.. code-block:: console

  git clone https://github.com/icecube/pal
  cd pal
  git checkout unstarlink
  ./bootstrap
  ./configure --prefix=/usr/local
  make
  make install


See Also
--------
- `cvmfs script <https://github.com/WIPACrepo/cvmfs/blob/master/builders/tools/pal.py>`_
- `homebrew script <https://github.com/icecube/homebrew-icecube/blob/master/pal.rb>`_
