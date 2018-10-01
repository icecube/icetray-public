Using the Intel C++ compiler
============================

Intel's C++ compiler is said to generate faster code than gcc and is free for 
noncommerical use on Linux platforms. Available here:

  http://software.intel.com/en-us/c-compilers/

Configuration
-------------

Specify the path to ``icc`` and ``icpc`` on the command line at cmake-time:

::

  % CC=/opt/intel/cc/10.1.018/bin/icc CXX=/opt/intel/cc/10.1.018/bin/icpc cmake ../src
  -- The C compiler identification is Intel
  -- The CXX compiler identification is Intel
  -- Check for working C compiler: /opt/intel/cc/10.1.018/bin/icc
  -- Check for working C compiler: /opt/intel/cc/10.1.018/bin/icc -- works
  -- Detecting C compiler ABI info
  -- Detecting C compiler ABI info - done
  -- Check for working CXX compiler: /opt/intel/cc/10.1.018/bin/icpc
  -- Check for working CXX compiler: /opt/intel/cc/10.1.018/bin/icpc -- works
  -- Detecting CXX compiler ABI info
  -- Detecting CXX compiler ABI info - done

Gotchas
-------

__sync_fetch_and_add
^^^^^^^^^^^^^^^^^^^^

See this thread:

  http://software.intel.com/en-us/forums/topic/304565

for details on a link error involving undefined symbol ``__sync_fetch_and_add``,
which is found in gcc 4.2 stl headers.  The patch is in 
``cmake/tool-patches/common/I3/intel_compatibility.h``.

libimf.so
^^^^^^^^^

If you see::

  error while loading shared libraries: libimf.so: cannot open shared object file: No such file or directory

You need to add the intel compiler's ``lib`` directory to your
``LD_LIBRARY_PATH``.  If you do so be careful that you don't do it in
such a way that you clobber the ``LD_LIBRARY_PATH`` set by the ``env-shell.sh``
