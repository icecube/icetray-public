.. _debian:
.. _ubuntu:

Ubuntu or Debian Variants
^^^^^^^^^^^^^^^^^^^^^^^^^

**Its is highly recommended to use Long-Term Support (LTS) releases of Ubuntu.**

Using the Package Manager
"""""""""""""""""""""""""

On laptops or user-controlled workstations it is recommended to install
as much as possible through the package manager.  The following instructions
are for 20.04 LTS.

Minimal Install
...............

.. container:: wrapped-code

    apt-get install build-essential cmake libbz2-dev libgsl0-dev libcfitsio-dev
    libboost-system-dev libboost-thread-dev libboost-date-time-dev
    libboost-python-dev libboost-serialization-dev libboost-filesystem-dev 
    libboost-program-options-dev libboost-regex-dev libboost-iostreams-dev

Full Install
............

Projects/tools in combo not built with this minimal package installation:
* NoiseEngine (needs libhealpix-cxx-dev)
* dst (needs libhealpix-cxx-dev)
* astro (needs libstarlink-pal-dev)
* filterscripts-cxx (needs astro which needs starlink)
* hdfwriter (needs libhdf5-serial-dev)
* libarchive (needs libarchive-dev and libzstd-dev)
* millipede (needs libsuitesparse-dev)
* SPRNGRandomService (needs libsprng2-dev)
* wavedeform (needs libblas-dev liblapack-dev)
* wavereform (needs python3-numpy-dev)
* steamshovel (needs libqt5opengl5-dev python3-matplotlib python3-pyqt5 ipython3)
* dataio-shovel (needs libcdk5-dev libncurses-dev)
* rootwriter (needs root-system)
* Muonitron table-maker (needs python3-numpy-dev)
* docs (needs python-sphinx doxygen)
* gcdserver (needs pymongo)
* unit tests (some needs python3-scipy)
* test coverage (needs lcov and python3-coverage)
  
.. container:: wrapped-code

    apt-get install build-essential cmake libbz2-dev libgsl0-dev libcfitsio-dev
    libboost-system-dev libboost-thread-dev libboost-date-time-dev
    libboost-python-dev libboost-serialization-dev libboost-filesystem-dev 
    libboost-program-options-dev libboost-regex-dev libboost-iostreams-dev
    libhealpix-cxx-dev libstarlink-pal-dev libhdf5-dev libzstd-dev
    libsuitesparse-dev libsprng2-dev liblapack-dev python3-numpy
    libqt5opengl5-dev libcdk5-dev libncurses-dev python3-sphinx doxygen
  
Special Install
...............

The following are a bit special and require extra care, especially if you
want to run clsim and ppc on GPUs, which require hardware drivers.

* ROOT (no longer provided via aptitude)
* clsim (needs OpenCL, ZMQ, and optionally GEANT)
  - libzmq5-dev
  - opencl-dev
* ppc (needs OpenCL)
  - opencl-dev
* g4-tankresponse (needs GEANT)

	       
