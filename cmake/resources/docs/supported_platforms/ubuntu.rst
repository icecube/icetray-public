.. _debian:
.. _ubuntu:

Ubuntu or Debian Variants
^^^^^^^^^^^^^^^^^^^^^^^^^

**It is highly recommended to use Long-Term Support (LTS) releases of Ubuntu.**

Using the Package Manager
"""""""""""""""""""""""""

On laptops or user-controlled workstations it is recommended to install
as much as possible through the package manager.  The following instructions
are for 20.04 LTS.

Minimal Install
...............
If any of the following packages are not installed ``cmake`` will complain and the build will fail.
This minimal installation may or may not work for you depending on what you're trying to do.
It's **highly** recommended you prefer the full/maximal install below.

.. code-block:: console

    apt-get install build-essential cmake libbz2-dev libgsl0-dev libcfitsio-dev
    libboost-system-dev libboost-thread-dev libboost-date-time-dev
    libboost-python-dev libboost-serialization-dev libboost-filesystem-dev
    libboost-program-options-dev libboost-regex-dev libboost-iostreams-dev
    python3-numpy fftw3-dev libboost-python-numpy

Full Install (**RECOMMENDED**)
..............................
With the exception of a few special projects below, the full install is the quickest and easiest way
to ensure IceTray will work for your purposes.

Projects/tools in combo not built with the minimal package installation above:

* NoiseEngine (needs libhealpix-cxx-dev)
* dst (needs libhealpix-cxx-dev)
* astro (needs libstarlink-pal-dev)
* filterscripts-cxx (needs astro which needs libstarlink-pal-dev)
* hdfwriter (needs libhdf5-serial-dev)
* libarchive (needs libarchive-dev and libzstd-dev)
* millipede (needs libsuitesparse-dev)
* SPRNGRandomService (needs libsprng2-dev)
* wavedeform (needs libblas-dev liblapack-dev)
* steamshovel (needs libqt5opengl5-dev, python3-matplotlib, python3-pyqt5, and ipython3)
* dataio-shovel (needs libcdk5-dev libncurses-dev)
* rootwriter (needs root-system)
* docs (needs python-sphinx doxygen)
* gcdserver (needs pymongo and python3-mysqldb)
* unit tests (some need python3-scipy)
* test coverage (needs lcov and python3-coverage)
* ml_suite (needs libboost-python-numpy, python3-h5py, python3-pandas, and python3-seaborn)

.. code-block:: console

    apt-get install build-essential cmake libbz2-dev libgsl0-dev
    libcfitsio-dev libboost-all-dev libstarlink-pal-dev libhdf5-dev
    libzstd-dev libsuitesparse-dev libsprng2-dev liblapack-dev
    python3-numpy fftw3-dev libqt5opengl5-dev libcdk5-dev libncurses-dev
    python3-sphinx doxygen python3-mysqldb python3-zmq python3-h5py
    python3-pandas python3-seaborn

Special Install
...............

The following are a bit special and require extra care, especially if you
want to run clsim and ppc on GPUs, which require hardware drivers.

* ROOT (no longer provided via aptitude)

  * libtbb-dev

* clsim (needs OpenCL, ZMQ, and optionally GEANT)

  * libzmq5-dev
  * python3-zmq
  * opencl-dev

* ppc (needs OpenCL)

  * opencl-dev

* g4-tankresponse (needs GEANT)
* ml_suite (tensorflow is optional)

  * https://www.tensorflow.org/install
