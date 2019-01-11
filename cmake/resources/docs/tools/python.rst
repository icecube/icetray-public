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
