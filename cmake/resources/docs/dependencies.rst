
Dependencies
============

Required Dependencies
---------------------

These packages are required to compile IceTray

.. list-table::  
  :widths: 4 10 10
  :header-rows: 1

  * - Name
    - Description
    - Documentation
  * - `CMake <https://cmake.org/>`_
    - Cross-platform build system
    - :doc:`CMake </projects/cmake/tutorial>`
  * - `python <https://www.python.org/>`_
    - Interpreted programming language
    - :doc:`/projects/cmake/tools/python`
  * - `NumPy <https://numpy.org/>`_
    - Python support for multi-dimensional arrays
    - :doc:`/projects/cmake/tools/python`
  * - `Boost <https://www.boost.org/>`_
    -  Boost C++ Libraries
    - :doc:`/projects/cmake/tools/boost`
  * - `GSL <https://www.gnu.org/software/gsl/>`_
    - GNU Scientific Library
    -  
  * - `CFITSIO <https://heasarc.gsfc.nasa.gov/fitsio/>`_
    - A FITS File Subroutine Library
    - 
  * - `zlib <https://zlib.net/>`_
    - Compression Algorithm
    - :doc:`/projects/dataio/tray`
  * - `fftw <http://www.fftw.org/>`_
    - Discrete Fourier transform
    - :ref:`dataclasses`

Optional Build Dependencies
---------------------------

These dependencies are optional. If not present IceTray will compile with reduced functionallity. 

.. list-table::  
  :widths: 2 10 5
  :header-rows: 1

  * - Name
    - Description
    - Needed for
  * - `libarchive <https://www.libarchive.org/>`_
    - Multi-format archive and compression library 
    - bz2 and tar support for i3 files
  * - `Zstandard <https://facebook.github.io/zstd/>`_
    - Compression Algorithm
    - zstd compressed i3 files
  * - `BLAS <https://netlib.org/blas/>`_
    - Basic Linear Algebra Subprograms
    - :ref:`millipede-main`, :ref:`wavedeform-main`
  * - `LAPACK <https://netlib.org/lapack/>`_
    - Linear Algebra PACKage
    - :ref:`millipede-main`, :ref:`wavedeform-main`, :ref:`I3GulliverLBFGSB <lilliput>`
  * - `SuiteSparse <https://people.engr.tamu.edu/davis/suitesparse.html>`_
    - Sparse matrix algorithms
    - :ref:`millipede-main`, :ref:`wavedeform-main`, :ref:`RoundTripChargeTest <DOMLauncher>`
  * - `Photospline <https://github.com/icecube/photospline>`_
    - Tensor-product B-spline surface evaluation
    - ``LeptonInjector``, :ref:`MuonGun-main`, :ref:`neutrino-generator`, :ref:`photonics-service`, :ref:`phys-services <phys-services>`, :ref:`spline-reco <spline-reco-main>`
  * - `ERFA <https://github.com/liberfa/erfa>`_
    - Essential Routines for Fundamental Astronomy
    - :ref:`astro`, :ref:`filterscripts`, :ref:`wimpsim-reader`
  * - `PAL <https://github.com/Starlink/pal>`_
    - Positional Astronomy Library
    - :ref:`astro`, :ref:`filterscripts`, :ref:`wimpsim-reader`
  * - `HDF5 <https://www.hdfgroup.org/solutions/hdf5/>`_
    - High-performance data management and storage suite
    - :ref:`hdfwriter`
  * - `Boost NumPy <https://www.boost.org/doc/libs/release/libs/python/doc/html/numpy/>`_
    - Boost.Python NumPy extension
    - :ref:`MuonGun-main` and ``ml_suite`` pybindings
  * - `HEALPix <https://healpix.jpl.nasa.gov/>`_
    - Hierarchical Equal Area isoLatitude Pixelization of a sphere
    - :ref:`NoiseEngine-main`, :ref:`dst`
  * - `SPRNG <http://www.sprng.org/>`_
    - Scalable Parallel Random Number Generators Library 
    - :doc:`SPRNGRandomService </projects/phys-services/distributed_rng>`
  * - `ROOT <https://root.cern/>`_
    - Particle physics data analysis 
    - :ref:`rootwriter`, :ref:`I3GulliverMinuit2 <lilliput>`
  * - `Minuit2 <https://root.cern/topical/#minuit>`_
    - Physics analysis tool for function minimization.
    - :ref:`I3GulliverMinuit2 <lilliput>`
  * - `CLHEP <https://proj-clhep.web.cern.ch/proj-clhep/>`_
    - A Class Library for High Energy Physics 
    - :ref:`clsim-main`, :ref:`g4-tankresponse-main`
  * - `Geant4 <https://geant4.web.cern.ch/>`_
    - Simulation of the passage of particles through matter
    - :ref:`clsim-main`, :ref:`g4-tankresponse-main`
  * - `OpenCL <https://www.khronos.org/opencl/>`_
    - framework for programs that execute across heterogeneous platforms
    - :ref:`clsim-main`, :ref:`ppc <ppc-main>`
  * - `GMP <https://gmplib.org/>`_
    - GNU Multiple Precision Arithmetic Library
    - :ref:`make_safeprimes <clsim-main>`
  * - `ZeroMQ <https://zeromq.org/>`_
    - An open-source universal messaging library
    - :ref:`clsim client/server <clsim-main>`
  * - `MultiNest <https://github.com/JohannesBuchner/MultiNest>`_
    - Bayesian inference tool for cosmology and particle physics
    - :ref:`I3GulliverMN <lilliput>`
  * - `NLOpt <https://nlopt.readthedocs.io/en/latest/>`_
    - library for nonlinear optimization
    - :ref:`I3GulliverNLopt <lilliput>`
  * - `ncurses <https://invisible-island.net/ncurses/>`_
    - Text-based user interfaces
    - :doc:`dataio-shovel </projects/dataio/shovel>`
  * - `CDK <https://invisible-island.net/cdk/>`_
    - Curses Development Kit 
    - :doc:`dataio-shovel </projects/dataio/shovel>`
  * - `OpenGL <https://www.opengl.org/>`_
    - 3D vector graphics
    - :ref:`steamshovel <steamshovel>`
  * - `GLUT <http://freeglut.sourceforge.net/>`_
    - OpenGL Utility Toolkit
    - :ref:`steamshovel <steamshovel>`
  * - `Qt5 <https://www.qt.io/>`_
    - GUI Widget Toolkit
    - :ref:`steamshovel <steamshovel>`

Other Dependencies
------------------

These are not needed at compile time but are necessary for certain functionality

.. list-table:: Required 
  :widths: 1 5 2
  :header-rows: 1

  * - Name
    - Description
    - Needed for
  * - `SciPy <https://scipy.org/>`_
    - Python library used for scientific computing
    - various unit tests
  * - `h5py <https://www.h5py.org/>`_
    - Pythonic interface to the HDF5 binary data format
    - various unit tests
  * - `PyTables <https://www.pytables.org/>`_
    - Python library managing hierarchical datasets
    - various unit tests
  * - `healpy <https://healpy.readthedocs.io/en/latest/>`_
    - Python interface for HEALPix C++ library
    - :ref:`dst`, unit tests
  * - `iminuit <https://iminuit.readthedocs.io/en/stable/>`_
    - Python interface for Minuit2 library
    - :ref:`lilliput`
  * - `pyzmq <https://pyzmq.readthedocs.io/en/latest/>`_
    - Python bindings for ØMQ.
    - :ref:`gcdserver`
  * - `scikit-learn <https://scikit-learn.org>`_
    - machine learning library 
    - :ref:`finallevel_filter_diffusenumu-main`
  * - `pymongo <https://pymongo.readthedocs.io/en/stable/>`_
    - Python bindings for MongoDB, 
    - :ref:`gcdserver`
  * - `Doxygen <https://www.doxygen.nl/index.html>`_
    - Generate documentation from source code
    - :doc:`documentation </projects/docs/index>`
  * - `Sphinx <https://www.sphinx-doc.org/en/master/>`_
    - intelligent and beautiful documentation
    - :doc:`documentation </projects/docs/index>`
  * - `Breathe <https://breathe.readthedocs.io/en/latest/>`_
    - Bridge between the Sphinx and Doxygen documentation systems
    - :doc:`documentation </projects/docs/index>`
  * - `Napoleon <https://sphinxcontrib-napoleon.readthedocs.io/en/latest/>`_
    - Legible docstrings for python
    - :doc:`documentation </projects/docs/index>`
