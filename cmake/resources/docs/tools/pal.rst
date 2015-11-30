PAL
===

PAl stands for Positional Astronomy Library it provides standard routines
for astronomical calculations. IceCube's astronomical coordinate software
`astro <../../astro/index.html>`_ requires either SLALIB or PAL to work. SLALIB is an old Fortran
library which was translated into c, it is no longer updated and has a weird
license and is hard to find. SLALIB can be installed is by
the `I3_PORTS <../ports.html>`_ system. The PAL source code is available on `github <https://github.com/Starlink/pal>`_,
but it is difficult to compile. A fork provided by IceCube which is much easier
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

PAL depends on a library called ERFA which can be downloaded from github and installed:

.. code-block:: bash

  git clone https://github.com/liberfa/erfa
  cd erfa
  ./bootstrap.sh
  ./configure
  make
  make install
  
PAL can be installed by downloading from github and compiling:

.. code-block:: bash
		
  git clone https://github.com/IceCube-SPNO/pal
  cd pal
  ./bootstrap 
  ./configure --prefix=/usr/local
  make
  make install

  
