.. highlight:: bash
	       
==========
Arch Linux
==========

This document describes the installation of dependencies needed for icetray on `Arch Linux <https://www.archlinux.org/>`_ via its package manager `pacman <https://wiki.archlinux.org/index.php/pacman>`_.
Arch is not an officially supported platform for icetray, but it is known to work on Arch.
If you are unsure what linux distro to use, do not pick Arch based on the existence of this page, you will almost certainly be happier with :ref:`Ubuntu <ubuntu>`. Feel free to ask questions about using icetray on Arch, but do not act entitled to support.

Many of the dependencies are available from the official Arch repositories. However, many icetray dependencies are only available through the `Arch user repositories <https://wiki.archlinux.org/index.php/Arch_User_Repository>`_ (AUR), so you will need an `AUR helper <https://wiki.archlinux.org/index.php/AUR_helpers>`_. This tutorial assumes that ``pacaur`` is your AUR helper. If you prefer a different AUR helper, please make the appropriate substitutions.

.. note::

   If you are on a machine that has more than one core, you can increace the
   speed that AUR packages are compiled by enableing will speed up things
   if you enable parallel builds: add ``MAKEFLAGS="-j$(nproc)"`` to your
   ``makepkg.conf`` before compiling

Base Dependancies
-----------------

First install the basic dependencies::

  pacman -S base-devel subversion cmake boost python zstd gsl cfitsio hdf5

Icetray depends on `BLAS <http://www.netlib.org/blas/>`_ library of which there are several implementations. By default Arch installs reference implementation of BLAS, which is quite slow.
`OpenBLAS <http://www.openblas.net/>`_ is much faster, but only available through the AUR::

  pacaur -S openblas-lapack

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

  pacaur -S healpix libcdk nlopt minuit2 icecube-pal-git python-mysqlclient

Building Documentation
----------------------

If you want to build icetray documentation you will need doxygen and sphinx::

  pacman -S doxygen python-sphinx 

OpenCL
------

icetray needs `OpenCL <https://www.khronos.org/opencl/>`_ for :ref:`clsim-main` and :ref:`ppc <ppc-main>`. Arch has a number of OpenCL implementations, you need to pick the one specific to your machine's GPU:

For NVIDIA use::

  pacman -S opencl-nvidia

For AMD/Radeon use::

  pacman -S opencl-mesa

If you don't have a GPU (possibly because you are in a virtualbox), a software implementation is available::

  pacman -S beignet

There are other alternatives available in the AUR see the `ArchWiki <https://wiki.archlinux.org/index.php/GPGPU#OpenCL>`_ for more details.

Packages that Need Modification
-------------------------------

Geant4
""""""

The AUR script for ``geant4`` turns on multithreading which doesn't play nice
with icetray. In order to turn it off you need to edit the PKGBUILD::

  pacaur -Se geant4

when the PKGBUILD comes up change the option ``-DGEANT4_BUILD_MULTITHREADED=ON``
to ``OFF``

ROOT
""""

``ROOT`` is also available in the AUR, but ROOT has some headerfile stupidness
that needs to be corrected *after* installation::

  pacaur -S root

once this is completed, you have to edit one of the header files it installed::

  sudo nano /usr/include/root/RConfigure.h

and change ``#define R__HAS_STD_EXPERIMENTAL_STRING_VIEW`` to ``#undef``


Linuxbrew
---------

The homebrew formulas produced by IceCube for use on OSX can also be used in linux with linuxbrew. Theses can be used for packages for which there are no Arch or AUR packages available. ::
  
  pacman -S ruby
  sh -c "$(curl -fsSL https://raw.githubusercontent.com/Linuxbrew/install/master/install.sh)"
  export PATH="/home/linuxbrew/.linuxbrew/bin:${PATH}"
  brew tap IceCube-SPNO/homebrew-icecube
  brew install --ignore-dependencies sprng2 multinest

Unsupported Packages
--------------------

``genie`` remains unsupported at this time.

 

