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

.. code::

   $ cmake ../src -DPython_EXECUTABLE=/path/to/python/executable

.. important::

   You should be using Python3. Support for Python2 was dropped at the beginning of 2021.

Setting Up Additional Python Tools
----------------------------------

There are several useful python tools that can be added to your local
environment to provide analysis-level tools like IPython
(https://ipython.org/), MatPlotLib
(https://matplotlib.org/), Numpy (https://numpy.org/)
and PyTables (https://www.pytables.org/). These instructions will help
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

*This section has become grossly out-of-date and has been removed. A new test-drive tutorial is welcome!*
