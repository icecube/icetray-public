.. index:: python
   pair:   tool; python
   single: python; using alternate

.. highlight:: sh

.. index:: PythonTool
.. _PythonTool:


Python
======

We use the *system* python3.  By default this is the one in
``/usr/bin``.  The development packages are required, eg the file
``/usr/include/python3.6/Python.h`` and so forth.  After you have
ensured that you have python and your development packages installed,
you will probably want to set up additional python tools.

To build against non-system python:

.. code:: bash
   cmake ../src -DPYTHON_EXECUTABLE=/path/to/python/executable

**Note**: It is strongly recommended that you use Python3.  Python2
will continue to be supported through 2020, but dropped in 2021.

Setting Up Additional Python Tools
----------------------------------

There are several useful python tools that can be added to your local
environment to provide analysis-level tools like IPython
(http://ipython.scipy.org/), MatPlotLib
(http://matplotlib.sourceforge.net/), Numpy (http://numpy.scipy.org/)
and PyTables (http://www.pytables.org/). These instructions will help
you get these additional tools setup and guide you through a few
examples.

Ubuntu or Debian Linux
^^^^^^^^^^^^^^^^^^^^^^

On recent Ubuntu (7.10 or better) and Debian (5.0 or better) Linux
releases, the system provided python tools work very well.  Install
the following packages and their dependencies:

* ipython
* python-matplotlib
* python-tables
* python-scipy

Mac OS X
^^^^^^^^

See :ref:`osxpythonsetup` for tips on setting up your Python environment. The
following packages are quite helpful and can be installed with :command:`pip`:

* ipython
* matplotlib
* pytables
* numpy
* scipy


FreeBSD
^^^^^^^

The python modules in ports work quite well. The recommended set is:

* devel/ipython
* math/py-matplotlib
* devel/py-tables
* science/py-scipy

Site CMake
^^^^^^^^^^

You will *need* to have a site cmake file for python, or else cmake
will try to use your system python and you will all sorts of ugly
errors.  More information can be found in the :ref:`I3_SITE_CMAKE_DIR`
section (which contains a python specific example.)

Taking your new python tools for a test-drive
---------------------------------------------
All source packages referenced below are available at:

http://code.icecube.wisc.edu/tools/distfiles/i3python/Test

1. Test things out (part 1)::

     > ipython

     In [1]: import numpy

     In [2]: help(numpy)

   Should see something like::

     NAME
       numpy 
 
     FILE
       /Users/blaufuss/python/lib/numpy-1.3.0-py2.5-macosx-10.5-i386.egg/numpy/__init__.py

     ..

   Can try the same thing with tables and matplotlib

2. Let's test things out (part 2):
   Get a checkout of offline-software/trunk::

      URL: http://code.icecube.wisc.edu/svn/meta-projects/offline-software/trunk

   In your src directory there, you need to the "booking project"::

     > svn co http://code.icecube.wisc.edu/svn/projects/booking/trunk booking

   The booking project contains the hdf5booker.  Cmake a buildspace, 
   and build as usual.  After you build, etc you'll have the 
   hdf5booker in your build/bin directory.

   Get ``PFFilt_L2_burn_run110770_test.i3.gz`` file and 
   after you've "./env-shell.sh"::

     > hdf5booker ./PFFilt_L2_burn_run110770_test.i3.gz

   This will generate::

      PFFilt_L2_burn_run110770_test.hdf5

   Get Zenith.py and::

      > ipython -pylab Zenith.py

   Should put up a plot of reconstructed zeniths from the L2 file 
   and plop a copy down in disk.



