.. index:: Red Hat Variants
.. _RedHatVariants:

Red Hat Variants
^^^^^^^^^^^^^^^^

CentOS
""""""

CentOS 7/8 are considered the official platform of IceCube, since it's widely
used in production.

Minimal Install
...............

Need to enable PowerTools to get boost-python3-devel.  Note that the stock
cmake version 3.11 doesn't detect python 3.6, so it's currently recommended
to install the lastest version of cmake (https://cmake.org/install/).


.. container:: wrapped-code

    dnf group install 'Development Tools'
    dnf config-manager --set-enabled PowerTools
    dnf install https://dl.fedoraproject.org/pub/epel/epel-release-latest-8.noarch.rpm

    yum install zlib-devel python36-devel
    boost-devel boost-system boost-thread boost-date-time
    boost-python boost-filesystem 
    boost-program-options boost-regex boost-iostreams
    boost-python3-devel
    gsl-devel bzip2-devel
    cfitsio-devel

    
Full Install
............

Projects/tools in combo not built with this minimal package installation:
* NoiseEngine (needs libhealpix-cxx)
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

  
Special Install
...............

The following are a bit special and require extra care, especially if you
want to run clsim and ppc on GPUs, which require hardware drivers.

* ROOT (no longer provided via aptitude)
* clsim (needs OpenCL, ZMQ, and optionally GEANT)
  - zmq5
  - opencl
* ppc (needs OpenCL)
  - opencl
* g4-tankresponse (needs GEANT)
