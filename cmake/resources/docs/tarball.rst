.. index:: 
   single: tarball
   single: grid, running on
   single: cluster, running on
   single: make targets ; tarball

Building tarballs for use on clusters or the grid
=================================================

``make tarball`` will build an tarball suitable for distribution to
the grid.  The unpacked tarballs have a similar layout to the
workspace one finds in one's :envvar:`I3_BUILD` build directory after
a build, with a few exceptions:

1.  In the tarballing process, the dependencies of whatever libraries
    exist in :file:`$I3_BUILD/lib` have been pulled in to the tarball,     
    in the directory :file:`$I3_BUILD/lib/tools`

#.  The env-shell.sh file is different: it determines the value 
    of :file:`$I3_BUILD` from its own location.

What this means is that these tarballs are relocatable: you should be
able to move them to another machine or another directory and run
from there.   

What is done
------------

The following is done in building a tarball:

.. _CMAKE_INSTALL_PREFIX:
.. index:: CMAKE_INSTALL_PREFIX

* The 'make install' target is executed, which installs the locally
  built libraries and scripts to the install directory (which is a
  subdirectory of the build directory containing metaproject and
  version number, like
  *offline-software.trunk.r49711.Linux-i686.gcc-4.2.3*.  This can be
  changed by setting ``CMAKE_INSTALL_PREFIX`` to a relative path (to
  the build directory), or an absolute path.  

* The built project libraries are scanned for dependencies on tools
  libraries, those are copied to a subdirectory ``tools/`` of  
  ``I3_BUILD/CMAKE_INSTALL_PREFIX/lib/``.

* A (filesystem) relocatable :ref:`env-shell.sh` is generated and
  placed into the install directory.  The following table shows the
  values that need prepending to the associated environment
  variables (where ``TBDIR`` is the root
  directory of the tarball):

  ===================== ==============================================
  Variable              Prepend
  ===================== ==============================================
  ``LD_LIBRARY_PATH``   ``$TBDIR/lib/:$TBDIR/lib/tools/``
  ``PATH``		``$TBDIR/bin/``
  ``PYTHONPATH``        ``$TBDIR/lib/``
  ===================== ==============================================
  
* *tarball-hook.sh* is run, if it exists (see below)

* The directory is tarred up and a checksum is generated.



Example
-------

.. highlight:: sh

::

   % make tarball
   Tarballing offline-software.trunk.r47978.Linux-i686.gcc-4.2.3
   Building installation directory offline-software.trunk.r47978.Linux-i686.gcc-4.2.3...
   Scanning dependencies of target env-check
   [  0%] Checking build against environment
   [  0%] Built target env-check
   [  5%] Built target boost_python
   
      [ etc etc ]

   [100%] Built target coordinate_service-pybindings
   [100%] Built target DOMcalibrator
   Install the project...
   -- Install configuration: "Debug"
   -- Installing: offline-software.trunk.r47978.Linux-i686.gcc-4.2.3/lib/icecube/__init__.py
   -- Installing: offline-software.trunk.r47978.Linux-i686.gcc-4.2.3/./env-shell.sh
   -- Installing: offline-software.trunk.r47978.Linux-i686.gcc-4.2.3/./log4cplus.conf
   
      [ etc etc ]

   -- Installing: offline-software.trunk.r47978.Linux-i686.gcc-4.2.3/include/DOMcalibrator/I3DOMcalibrator.h
   -- Installing: offline-software.trunk.r47978.Linux-i686.gcc-4.2.3/lib/libDOMcalibrator.so
   Built target install_tool_libs
   Generating BUILD_INFO
   Adding svn info to tarball
   Tarball hook script 'tarball_hook.sh' exists in metaproject directory.
   Running tarball_hook.sh
   Running tarball_hook.sh to tweak installation at offline-software.trunk.r47978.Linux-i686.gcc-4.2.3
   Creating offline-software.trunk.r47978.Linux-i686.gcc-4.2.3.tar.gz
   Checksumming offline-software.trunk.r47978.Linux-i686.gcc-4.2.3.tar.gz
   Built target tarball

now to exit our environment and untar this someplace else::

   % exit
   Exited Icetray Environment.
   % cp offline-software.trunk.r47978.Linux-i686.gcc-4.2.3.tar.gz /tmp
   % cd /tmp
   % mkdir instdir
   % cd instdir 
   % tar xvzf ../offline-software.trunk.r47978.Linux-i686.gcc-4.2.3.tar.gz 
   offline-software.trunk.r47978.Linux-i686.gcc-4.2.3/
   offline-software.trunk.r47978.Linux-i686.gcc-4.2.3/twr-decode/
   offline-software.trunk.r47978.Linux-i686.gcc-4.2.3/twr-decode/resources/
   offline-software.trunk.r47978.Linux-i686.gcc-4.2.3/twr-decode/resources/test_timing_period2_ChannelID_TWRKey.xml
      [etc etc]

   offline-software.trunk.r47978.Linux-i686.gcc-4.2.3/include/DOMcalibrator/I3DOMcalibrator.h

Note that the tools libraries have been pulled in to the tarball::

   % ls offline-software.trunk.r47978.Linux-i686.gcc-4.2.3/lib/tools 
   libCint.so@             libRint.so.5.18*
   libCint.so.5@           libTree.so@
   libCint.so.5.18*        libTree.so.5@
   ...

And that the python is in :file:`lib/icecube`::

   % ls lib/icecube 
   __init__.py            dataclasses.so  icetray/       phys_services.so
   __init__.pyc           dataio.so       icetray.so     util/
   cfirst.so              examples/       interfaces.so
   coordinate_service.so  examples.so     jebclasses.so

Now we can run the env-shell.sh script that is in there and use the software::

   % ./offline-software.trunk.r47978.Linux-i686.gcc-4.2.3/env-shell.sh 
   ************************************************************************
   *                                                                      *
   *                   W E L C O M E  to  I C E T R A Y                   *
   *                                                                      *
   *              Version offline-software.trunk     r47978               *
   *                                                                      *
   *                You are welcome to visit our Web site                 *
   *                        http://icecube.umd.edu                        *
   *                                                                      *
   ************************************************************************
   
   Icetray environment has:
      I3_SRC       = /tmp/instdir/offline-software.trunk.r47978.Linux-i686.gcc-4.2.3
      I3_BUILD     = /tmp/instdir/offline-software.trunk.r47978.Linux-i686.gcc-4.2.3
   % python
   Python 2.5.2 (r252:60911, Jul 31 2008, 17:28:52) 
   [GCC 4.2.3 (Ubuntu 4.2.3-2ubuntu7)] on linux2
   Type "help", "copyright", "credits" or "license" for more information.
   >>> from icecube import icetray, dataclasses, dataio
   >>> rhs = dataclasses.I3RecoHitSeriesMap()
   >>> 

Note:

#.  One needn't be in the same directory where the tarball is unpacked.



.. index:: tarball_hook.sh

For metaproject maintainers:  tarball_hook.sh
---------------------------------------------

There is a script in the build space called
:file:`tarball_hook.sh.in`::

    #!/bin/sh
    #
    # THIS IS AN AUTOGENERATED FILE, ANY CHANGES HERE WILL BE LOST.
    # LOOK IN THE METAPROJECT SOURCE DIRECTORY TO FIND THE PRECONFIGURED
    # VERSION OF THIS FILE, THAT'S THE ONE YOU WANT TO TWEAK.
    #
    # Sample tarball_hook script.  Modify as needed (but keep it tasteful)
    # and check in to your metaproject source directory.  It will get
    # configured by cmake and dropped into your build directory at cmake time.
    
    
    echo "Running tarball_hook.sh to tweak installation at @CMAKE_INSTALL_PREFIX@"
    
    #
    # add the crunchy bits to your tarball here.
    #
    uname -a > @CMAKE_INSTALL_PREFIX@/uname.txt
    
Which is copied to ``tarball_hook.sh`` when cmake runs, and
variables ``@INSIDE_AT_SIGNS@`` are expanded.  For instance
``@CMAKE_INSTALL_PREFIX@`` might become
``offline-software.trunk.r47978.Linux-i686.gcc-4.2.3``.  If your
metaproject requires certain last-minute tweaks before the tarball is
made (that is, after cmake has installed everything but before ``tar``
is actually run), you can commit to the copy ``tarball_hook.sh.in`` in
the toplevel directory of your metaproject.  The changes will then get
run each time a user makes ``tarball``.

Changing the name of the tarball
--------------------------------

.. index:: CMAKE_INSTALL_PREFIX

Set :data:`CMAKE_INSTALL_PREFIX` as you like in your
:file:`CMakeCache.txt`, or pass ``-DCMAKE_INSTALL_PREFIX=tarball_name`` when 
you invoke ``cmake``. If you want to use the tarball together with
``icetray-start``, pass ``-DCMAKE_INSTALL_PREFIX=tarball_name.${OS_ARCH}``.

Lightweight tarballs for CVMFS-enabled grids
--------------------------------------------

Some grids (e.g. OpenScienceGrid) support distribution of software via CVMFS,
which obviates much of the need for tarballs of standard metaprojects. When
the neded arises for custom metaprojects, however, their tarballs can omit the
libraries already in the CVMFS repository. To build a lightweight tarball, set
the :data:`INSTALL_TOOL_LIBS` option to OFF at configure.  RPATH entries
will not be stripped from the installed libraries.

