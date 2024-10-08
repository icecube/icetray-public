.. SPDX-FileCopyrightText: 2024 The IceTray Contributors
..
.. SPDX-License-Identifier: BSD-2-Clause

.. index:: Red Hat Variants
.. _RedHatVariants:

Red Hat Variants
^^^^^^^^^^^^^^^^

CentOS / AlmaLinux / Rocky Linux
""""""""""""""""""""""""""""""""

Red Hat variants not recommended for desktop or laptop use.  Your
mileage may vary.  Please report successes and failures to the
#software channel.

.. note::

   As of December 2020, Red Hat unilaterally terminated CentOS
   development. The last version was CentOS 8.5. Rocky Linux and
   AlmaLinux are considered "spiritual successors".

Prerequisite for the optional module ``photospline``
""""""""""""""""""""""""""""""""""""""""""""""""""""

``photospline`` is no longer included as part of IceTray, but developed
independently, and must be installed before building IceTray.

.. code-block:: console

   $ git clone https://github.com/icecube/photospline.git
   $ mkdir photospline/build
   $ cd photospline/build
   $ cmake ..
   $ make -j$(nproc)
   $ sudo make install

For detailed installation instructions see https://github.com/icecube/photospline.

Minimal Install
...............

Need to enable PowerTools to get boost-python3-devel.  Note that the stock
cmake version 3.11 doesn't detect python 3.6, so it's currently recommended
to install the latest version of cmake (https://cmake.org/install/).


.. code-block:: console

    $ dnf install epel-release
    $ dnf config-manager --enable epel
    $ dnf config-manager --enable extras

    # for AlmaLinux 8
    $ dnf config-manager --set-enabled powertools

    # for AlmaLinux 9
    $ dnf config-manager --set-enabled crb

    $ yum install gcc gcc-c++ make
    $ yum install zlib-devel python39-devel \
      boost-devel boost-system boost-thread boost-date-time \
      boost-filesystem boost-program-options boost-regex boost-iostreams \
      boost-python3-devel gsl-devel bzip2-devel cfitsio-devel

Full Install
............

The following projects are considered optional and the system libraries have
not been tested yet.

Projects/tools in combo not built with this minimal package installation:

* hdfwriter (needs hdf5)
* millipede (needs suitesparse)
* wavereform (needs python3-numpy)
* Muonitron table-maker (needs python3-numpy)
* docs (needs python-sphinx doxygen)
* gcdserver (needs pymongo)
* unit tests (some needs python3-scipy)

Special Install
...............

The following are a bit special and require extra care, especially if you
want to run clsim and ppc on GPUs, which require hardware drivers.

* dataio-shovel (needs cdk5, ncurses)
* rootwriter (needs ROOT)
* steamshovel (needs qt5opengl5 python3-matplotlib python3-pyqt5 ipython3)
* SPRNGRandomService (needs libsprng2)
* wavedeform (needs blas lapack)
* libarchive (needs archive, zstd)
* NoiseEngine (needs healpix-cxx)
* dst (needs healpix-cxx)
* astro (needs starlink-pal)
* filterscripts-cxx (needs astro which needs starlink)
* ROOT (no longer provided via aptitude)
* clsim (needs OpenCL, ZMQ, and optionally GEANT)

  * zmq5
  * opencl

* ppc (needs OpenCL)

  * opencl

* g4-tankresponse (needs GEANT)
