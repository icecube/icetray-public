PAL
===

PAL stands for Positional Astronomy Library it provides standard routines
for astronomical calculations. The PAL library is a partial re-implementation
of Pat Wallace's popular SLALIB library written in C using a Gnu GPL license
and layered on top of the IAU's SOFA library (or the BSD-licensed ERFA) where
appropriate.  SLALIB is an old Fortran
library which was translated into c, it is no longer updated, has a weird
license, and is hard to find.
IceCube's astronomical coordinate software
`astro <../../astro/index.html>`_ requires PAL to work.
The PAL source code is available on `github <https://github.com/Starlink/pal>`_,
but it is difficult to compile. A `fork  <https://github.com/IceCube-SPNO/pal>`_
provided by IceCube which is much easier
to compile is available from the IceCube github repository,
it depends on a library called ERFA.

Apple Mac OS X
--------------

On MAC OS X, homebrew can be used:

.. code-block:: bash
		
  brew tap IceCube-SPNO/homebrew-icecube
  brew install pal

for other systems it must be installed from source.

Install from Source
--------------------

PAL depends on a library called ERFA which can be downloaded from github.

.. code-block:: bash

  git clone https://github.com/liberfa/erfa
  cd erfa
  ./bootstrap.sh
  ./configure
  make
  make install
  
For Ubuntu you may also have to install **autoconf** and **libtool**, depending on your 
specific distribution. :

PAL can be installed by downloading from github and compiling:

.. code-block:: bash
		
  git clone https://github.com/IceCube-SPNO/pal
  cd pal
  ./bootstrap 
  ./configure --prefix=/usr/local
  make
  make install

  
See Also
--------
- `cvmfs script <https://github.com/WIPACrepo/cvmfs/blob/master/builders/tools/pal.py>`_
- `homebrew script <https://github.com/IceCube-SPNO/homebrew-icecube/blob/master/pal.rb>`_
