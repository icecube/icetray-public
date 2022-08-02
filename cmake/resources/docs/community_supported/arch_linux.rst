.. highlight:: bash
       
==========
Arch Linux
==========

This document describes the installation of dependencies needed for icetray on `Arch Linux <https://archlinux.org/>`_ via its package manager `pacman <https://wiki.archlinux.org/title/pacman>`_.
Arch is not an officially supported platform for icetray, but it is known to work on Arch.
If you are unsure what linux distro to use, do not pick Arch based on the existence of this page, you will almost certainly be happier with :ref:`Ubuntu <ubuntu>`. Feel free to ask questions about using icetray on Arch, but do not act entitled to support.

Many of the dependencies are available from the official Arch repositories. However, many icetray dependencies are only available through the `Arch user repositories <https://wiki.archlinux.org/title/Arch_User_Repository>`_ (AUR), so you will need an `AUR helper <https://wiki.archlinux.org/title/AUR_helpers>`_. This tutorial assumes that ``paru`` is your AUR helper. If you prefer a different AUR helper, please make the appropriate substitutions.

.. note::

   If you are on a machine that has more than one core, you can increase the
   speed that AUR packages are compiled by enableing parallel builds: add
   ``MAKEFLAGS="-j$(nproc)"`` to your ``makepkg.conf`` before compiling.

Base Dependancies
-----------------

First install the basic dependencies::

  pacman -S base-devel cmake boost python zstd gsl cfitsio hdf5 zeromq fftw

Icetray depends on `BLAS <https://netlib.org/blas/>`_ library of which there are several implementations. By default Arch installs reference implementation of BLAS, which is quite slow.
`OpenBLAS <https://www.openblas.net/>`_ is much faster, but only available through the AUR::

  paru -S openblas-lapack

Once is OpenBLAS is installed you can install packages which depend on BLAS::

  pacman -S suitesparse python-numpy python-scipy python-matplotlib

and they will link against OpenBLAS

Steamshovel
-----------

The following packages are needed for :ref:`Steamshovel <steamshovel>`::

  pacman -S qt5-base freeglut python-qtconsole

.. note:: 

  ``steamshovel`` now uses QT5 for it's graphical interface which has builtin detection in cmake. However, it is still nescessary to have the old detection mechanism for QT4 present. This may cause a message like "``Found unsuitable Qt version "5.X.X" from /bin/qmake``", don't worry this can be safely ignored. As long as the steamshovel project configures without any error messages, steamshovel will be build with QT5.

Reccomended Packages
--------------------

The following reccomended packages are also available from the AUR::

  paru -S healpix libcdk nlopt starlink-pal python-healpy python-pymongo

Building Documentation
----------------------

If you want to build icetray documentation you will need ``doxygen``, ``sphinx``, ``breathe`` and ``sphinx``::

  paru -S doxygen python-sphinx python-breathe python-sphinxcontrib-napoleon

OpenCL
------

icetray needs `OpenCL <https://www.khronos.org/opencl/>`_ for :ref:`clsim-main` and :ref:`ppc <ppc-main>`. Arch has a number of OpenCL implementations, you need to pick the one specific to your machine's GPU:

For NVIDIA use::

  pacman -S opencl-nvidia

For AMD/Radeon use::

  pacman -S opencl-mesa

For Intel Iris use::

  pacman -s intel-compute-runtime ocl-icd

If you don't have a GPU (possibly because you are in a virtual machine), a software implementation is available but slow::

  pacman -S pocl

There are other alternatives available for different hardware in the AUR see the `ArchWiki <https://wiki.archlinux.org/title/GPGPU#OpenCL>`_ for more details.

Regardless of which implementation you pick you will also need the opencl-headers::
  
  pacman -S opencl-headers

Geant4
------

``geant4`` is available in the AUR but the datafiles are in different packages. 
The ones that are needed for icetray are::

  paru -S geant4 geant4-ensdfstatedata geant4-levelgammadata geant4-ledata geant4-particlexsdata

and you need to set the environment variables::

  export G4LEVELGAMMADATA=/usr/share/geant4-levelgammadata/PhotonEvaporation5.7
  export G4ENSDFSTATEDATA=/usr/share/geant4-ensdfstatedata/G4ENSDFSTATE2.3
  export G4LEDATA=/usr/share/geant4-ledata/G4EMLOW7.13/
  export G4PARTICLEXSDATA=/usr/share/geant4-particlexsdata/G4PARTICLEXS3.1

ROOT
----

ROOT is available in ``community`` but it is compiled with c++17 so you need to
compile icetray with c++17. This can be done by chaning the setting in
``cmake/config.cmake``::

  set(CMAKE_CXX_STANDARD 17)

Unsupported Packages
--------------------

``sprng2``, and ``multinest`` remain unsupported at this time.
