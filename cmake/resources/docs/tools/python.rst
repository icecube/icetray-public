.. index:: python
   pair:   tool; python
   single: python; using alternate

.. highlight:: sh

Python
======

We use the *system* python.  By default this is the one in
``/usr/bin``.  The development packages are required, eg the file
``/usr/include/python2.5/Python.h`` and so forth.  If you want to use
a python in a nonstandard location, tell *cmake* what these locations
are at configure time::

   % cmake -DPYTHON_EXECUTABLE=/path/to/my/bin/python2.5 \
       -DPYTHON_LIBRARIES=/path/to/my/lib/python2.5/config/libpython2.5.so \
       -DPYTHON_INCLUDE_DIR=/path/to/my/include/python2.5 \
       ../src

You should see these correctly reported in the output of the cmake
configuration step::

   -- python
   -- +  version: Python 2.5.2
   -- +   binary: /path/to/my/bin/python2.5
   -- + includes: /path/to/my/include/python2.5
   -- +     libs: /path/to/my/lib/python2.5/config/libpython2.5.so

.. warning::

   If this gets botched up, **start from a clean build space**,
   i.e. delete your ``CMakeCache.txt``.

You will probably need to have your ``PYTHONPATH`` set appropriately,
the ``env-shell.sh`` file will prepend to but not clobber this
variable.

Setting Up additional Python tools
-----------------------------------

There are several useful python tools that can be added to your local 
environment to provide analysis-level tools like IPython 
(http://ipython.scipy.org/), MatPlotLib 
(http://matplotlib.sourceforge.net/), Numpy (http://numpy.scipy.org/)
and PyTables (http://www.pytables.org/). These instructions will
help you get these additional tools setup and guide you through a few
examples.  

**Note**: At a minimum, you're going to need at Python 2.4 or better, and 
it is highly recommended you use Python 2.5 or better.

Ubuntu or Debian Linux
^^^^^^^^^^^^^^^^^^^^^^^
On recent Ubuntu (7.10 or better) and Debian (5.0 or better) Linux releases, the
system provided python tools work very well.  Install the following
packages and their dependencies:

* ipython
* python-matplotlib
* python-tables
* python-scipy

Mac OS X 10.5 Leopard
^^^^^^^^^^^^^^^^^^^^^^
**Note**: These instructions were put together to help get
the python tools installed on a Mac running 10.5 Leopard.  I've
tested them on a few different macs with good success.  Comments,
questions and corrections are encouraged (python@icecube.wisc.edu).

All source packages referenced below are available at:

http://code.icecube.wisc.edu/tools/distfiles/i3python/Mac/

0. Start with a fresh install of Mac OSX 10.5 with 
   the Xcode tools installed from the installation CD.

1. Setup I3_PORTS - The recent offline releases  (and trunk) use 
   the meta-package of tools ``i3-tools-v3``, so either get a fresh,
   new I3_PORTS install, or::

     > $I3_PORTS/bin/port sync
     > $I3_PORTS/bin/port install i3-tools-v3

   (includes new boost, new root, etc)

2. Setup setuptools to use your home directory::

     blaufuss@chiclets[~]% more .pydistutils.cfg
     [install]
     install_lib = ~/python/lib
     install_scripts = ~/python/bin

   Create these directories, if needed, and you 
   need to add these to your .bash_profile as::

     export PATH=~/python/bin:$PATH
     export PYTHONPATH=~/python/lib

   These settings need to be set in your environment before you continue.

3. Install setup tools.  Get ``setuptools-0.6c9-py2.5.egg`` and::

     > sh setuptools-0.6c9-py2.5.egg

   This will install setuptools, now easy_install should be in your path::

     blaufuss@chiclets[~]% which easy_install
     /Users/blaufuss/python/bin/easy_install

4. This one is optional, but very nice. Install 
   ipython (A much friendlier python front-end)::

     > easy_install readline

   (This fixes tab completion on the Mac) and::

     > easy_install ipython

5. Install Matplotlib.  Get ``matplotlib-0.91.4-py2.5-macosx-10.3.egg`` and::

       > easy_install  ./matplotlib-0.91.4-py2.5-macosx-10.3.egg

   **Note**:This version of the package seems to install correctly, then 
   will try to go off to the network and
   fetch the latest (currently, this one) version again and fail 
   with an error about getting HTML.  Ignore this
   second attempt, Not sure what's up, but it installed 
   fine on several machines with this error,
   you should see a matplotlib-0.91.4-py2.5-macosx-10.3.egg/ 
   entry in your ~/python/lib dir)

6. Install numpy. Get ``numpy-1.3.0.zip`` and::
    > easy_install ./numpy-1.3.0.zip

7. Intstall Pytables.  First, you need hdf5 libraries.  
   These are now in $I3_PORTS::

     > $I3_PORTS/bin/port install hdf5_1.6.8

   And now get ``tables-2.1.1.tar.gz`` and::

      > sh -c "export HDF5_DIR=/Users/blaufuss/icework/i3tools; ~/python/bin/easy_install ./tables-2.1.1.tar.gz"

Redhat/Scientific Linux 5
^^^^^^^^^^^^^^^^^^^^^^^^^
**Note**: These instructions were put together to help get
the python tools installed on Linux installation of RHEL 5.1. Comments,
questions and corrections are encouraged (python@icecube.wisc.edu).

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

7. Intstall Pytables.  First, you need hdf5 libraries.  
   These are now in $I3_PORTS::

     > $I3_PORTS/bin/port install hdf5_1.6.8

   And now get ``tables-2.1.1.tar.gz`` and::

      > sh -c "export HDF5_DIR=/Users/blaufuss/icework/i3tools; ~/python/bin/easy_install ./tables-2.1.1.tar.gz"


Taking your new python tools for a test-drive
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
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



