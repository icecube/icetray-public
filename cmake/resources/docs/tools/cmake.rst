.. index:: cmake
   pair:   tool; cmake
   
.. highlight:: none


cmake
=====

Unlike our other tools, each version of the cmake tool installs a
binary to :file:`$I3_PORTS/bin/cmake`, as well as a few others.  This
means that one cannot simultaneously install several versions of the
cmake tool.  If you attempt to e.g. install cmake 2.6.4 when cmake
2.6.0 is already installed, you'll see::

  % /opt/i3/ports/bin/port -v install cmake_2.6.4 

  [ build, build ... ]
  --->  Archive cmake_2.6.4-2.6.4_1.x86_64.tgz packaged
  --->  Archive for cmake_2.6.4 2.6.4_1 packaged
  --->  Installing cmake_2.6.4 2.6.4_1
  --->  Activating cmake_2.6.4 2.6.4_1
  Error: Target com.apple.activate returned: Image error: /opt/i3/ports/bin/cmake is being used by the active cmake_2.6.0 port.  Please deactivate this port first, or use the -f flag to force the activation.
  Warning: the following items did not execute (for cmake_2.6.4): com.apple.activate

In addition, several 'metatools' depend on cmake, so uninstalling the
older version probably won't 'just work'::

  % /opt/i3/ports/bin/port -v uninstall cmake_2.6.0 
  --->  Unable to uninstall cmake_2.6.0 2.6.0_1, the following ports depend on it:
  --->    i3-tools-v3
  --->    boost_1.38.0

The solution is to force the uninstall::

  % /opt/i3/ports/bin/port -vf uninstall cmake_2.6.0
  --->  Unable to uninstall cmake_2.6.0 2.6.0_1, the following ports depend on it:
  --->    i3-tools-v3
  --->    boost_1.38.0
  Warning: Uninstall forced.  Proceeding despite dependencies.
  --->  Deactivating cmake_2.6.0 2.6.0_1
  --->  Uninstalling cmake_2.6.0 2.6.0_1
  --->  Uninstall is removing cmake_2.6.0 from the port registry.
    
(note the -f flag).  Then you can install the newer cmake::

  % /opt/i3/ports/bin/port -v install cmake_2.6.4
  --->  Activating cmake_2.6.4 2.6.4_1
  --->  Cleaning cmake_2.6.4
  --->  Removing workpath for cmake_2.6.4

  % /opt/i3/ports/bin/cmake --version
  cmake version 2.6-patch 4
