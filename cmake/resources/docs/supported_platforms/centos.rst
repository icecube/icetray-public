.. index:: Red Hat Variants
.. _RedHatVariants:

Red Hat Variants
^^^^^^^^^^^^^^^^

CentOS
""""""

CentOS 7/8 is not yet recommended for desktop and laptop use.  Your mileage may vary.
Please report successes and failures to the #software channel.

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

The following projects are considered optional and the system libraries have
not been tested yet.

Projects/tools in combo not built with this minimal package installation:
* NoiseEngine (needs healpix-cxx)
* dst (needs healpix-cxx)
* astro (needs starlink-pal)
* filterscripts-cxx (needs astro which needs starlink)
* hdfwriter (needs hdf5)
* libarchive (needs archive, zstd)
* millipede (needs suitesparse)
* SPRNGRandomService (needs libsprng2)
* wavedeform (needs blas lapack)
* wavereform (needs python3-numpy)
* steamshovel (needs qt5opengl5 python3-matplotlib python3-pyqt5 ipython3)
* dataio-shovel (needs cdk5, ncurses)
* rootwriter (needs ROOT)
* Muonitron table-maker (needs python3-numpy)
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
