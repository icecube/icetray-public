.. index:: python
   pair:   tool; python
   single: python; using alternate

.. highlight:: sh

.. index:: PythonTool
.. _PythonTool:


Python
======

We use the *system* python.  By default this is the one in
``/usr/bin``.  The development packages are required, eg the file
``/usr/include/python2.6/Python.h`` and so forth.  If you want to use
a python in a nonstandard location refer to :ref:`ns-python`. After
you have ensured that you have python and your development packages
installed, you will probably want to set up additional python tools.

**Note**: Python 2.6 or newer (up to 3.3 has been tested) are *strongly*
recommended. Limited functionality may be available on Python 2.4 and 2.5.
Python older than 2.4 is completely unsupported and will not work.

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

See :ref:`osx-python-setup` for tips on setting up your Python environment. The
following packages are quite helpful and can be installed with :command:`pip`:

* ipython
* matplotlib
* pytables
* numpy
* scipy

Redhat/Scientific Linux 5
^^^^^^^^^^^^^^^^^^^^^^^^^

**Note**: These instructions were put together to help get the python
tools installed on Linux installation of RHEL 5.1. Comments, questions
and corrections are encouraged (python@icecube.wisc.edu).

All source packages referenced below are available at:

http://code.icecube.wisc.edu/tools/distfiles/i3python/Linux

0. A warning:  I'm sure I've not been comprehensive in generating
   the dependencies on system tools.  Your mileage may vary.

1. Setup I3_PORTS - The recent offline releases  (and trunk) use 
   the meta-package of tools ``i3-tools-v3``, so either get a fresh,
   new I3_PORTS install, or::

     > $I3_PORTS/bin/port sync
     > $I3_PORTS/bin/port install i3-tools-v3

   (includes new boost, new root, etc)

2. Setup setuptools to use your home directory::

     blaufuss@morax[~]% more .pydistutils.cfg
     [install]
     install_lib = ~/python/lib
     install_scripts = ~/python/bin

   Create these directories, if needed, and you 
   need to add these to your .bash_profile as::

     export PATH=~/python/bin:$PATH
     export PYTHONPATH=~/python/lib
     export LD_LIBRARY_PATH=$I3_PORTS/lib

   These settings need to be set in your environment before you continue.

   **Note**:  There is nothing special about these values set 
   in this ``~/.pydistutils``.  This file is used by the Python Setuptools
   to see where to install libraries for your use.  The ``.pydistutils.cfg``
   file has to be in your ``$HOME`` directory.  The values set in there can be
   any directory you can write to, as long as they match the values you set
   in ``PATH`` and ``PYTHONPATH``.

3. Install setup tools.  Get ``setuptools-0.6c9-py2.4.egg`` and::

     > sh setuptools-0.6c9-py2.4.egg

   This will install setuptools, now easy_install should be in your path::

     blaufuss@morax[~]% which easy_install
     /Users/blaufuss/python/bin/easy_install

4. This one is optional, but very nice. Install 
   ipython (A much friendlier python front-end)::

     > easy_install ipython

5. Install numpy. Get ``numpy-1.3.0.zip`` and::
    > easy_install ./numpy-1.3.0.zip

6. Install Matplotlib.  Get ``matplotlib-0.91.4.tar.gz`` and::

       > tar zxvf matplotlib-0.91.4.tar.gz
       > cd matplotlib-0.91.4
       > easy_install  ./

   **Note**: I had to install libpng-devel package to get this to compile.
   you should see a matplotlib-0.91.4-py2.4-linux-i686.egg
   entry in your ~/python/lib dir)

7. Install Pytables.  First, you need hdf5 libraries.  
   These are now in $I3_PORTS::

     > $I3_PORTS/bin/port install hdf5_1.6.8

   And now get ``tables-2.1.1.tar.gz`` and::

      > sh -c "export HDF5_DIR=/Users/blaufuss/icework/i3tools; ~/python/bin/easy_install ./tables-2.1.1.tar.gz"

FreeBSD
^^^^^^^

The python modules in ports work quite well. The recommended set is:

* devel/ipython
* math/py-matplotlib
* devel/py-tables
* science/py-scipy

.. _ns-python:

Using a non-system python
-------------------------

While using a non-system python is not difficult, it is more involved
than just using the python that comes pre-installed with your OS.
Sometimes though, it is unavoidable. Therefore we provide these
general guidelines.

Installing Ports
^^^^^^^^^^^^^^^^

This is the single most important step to success with using a
non-system python.

When installing ports *do not* follow the all of the instructions at
:ref:`installing-ports`. When you are asked to do::

    ./i3-install.sh $I3_PORTS

Instead, what you need to do is::

    ./configure --prefix=$I3_PORTS --with-tcl='path to directory containing tclConfig.sh' --with-python='path to python binary'

where ``'path to directory containing tclConfig.sh'`` is typically
``/usr/lib`` and ``'path to python binary'`` is the *full* path to the
non-system python executable that you want to use. (For example,
``/opt/python2.7/bin/python``)

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



