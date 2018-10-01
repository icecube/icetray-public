FAQ
===

.. highlight: none

**Q**::

    I updated some tools a time ago and now try to run some older (offline V1 based) software projects.
    
    When I try to run cmake I get the following error
    
    --   amanda-core
    CMake Error at cmake/tools.cmake:44 (message):
      Attempt to use tool 'rdmc' which wasn't found
    Call Stack (most recent call first):
      cmake/tools.cmake:75 (use_tool)
      cmake/project.cmake:171 (use_tools)
      amanda-core/CMakeLists.txt:6 (i3_add_library)
    
    A 'port installed' will show
    
      cmake_2.4.6 2.4.6_1
      cmake_2.6.0 2.6.0_1 (active)
      ...
      rdmc_2.9.5 2.9.5_3 (active)
    
    So RDMC is available, but this is the newer version (I think 2.9.3 was used in the older toolset)
    
    What is the recommended way of handling this problem?
    
    1.) Hacking src/cmake/tools/Findrdmc.cmake
    2.) Have several tool installations (How would I do this on a MAC)
    3.) other


**A**:

* #1 will work.  The ``Findrdmc.cmake`` in that metaproject probably is
  hardcoded to detect version 2.9.3.  This is so that results are
  reproducible... tool versions need to be stored in source control.
  You'd just change the `2.9.3` to `2.9.5`, hoping that whatever
  you're compiling works with the newer version.

* **recommended**: Rather than having several tool installations, you can simply install the
  `2.9.3` rdmc alongside the newer one.  ``port search`` will show what is available::

      % port search rdmc
      rdmc_2.9.3      icecube/rdmc_2.9.3      2.9.3           The AMANDA-era RDMC physics library
      rdmc_2.9.5      icecube/rdmc_2.9.5      2.9.5           The AMANDA-era RDMC physics library

  and you can install it manually::

      % sudo /opt/i3/ports/bin/port install rdmc_2.9.3
      --->  Fetching rdmc_2.9.3
      --->  Attempting to fetch rdmc-2.9.3.tar.gz from http://code.icecube.wisc.edu/tools/distfiles/
      rdmc
      --->  Verifying checksum(s) for rdmc_2.9.3
      --->  Extracting rdmc_2.9.3
      --->  Configuring rdmc_2.9.3
      --->  Building rdmc_2.9.3 with target all
      --->  Staging rdmc_2.9.3 into destroot
      --->  Packaging tgz archive for rdmc_2.9.3 2.9.3_2
      --->  Installing rdmc_2.9.3 2.9.3_2
      --->  Activating rdmc_2.9.3 2.9.3_2
      --->  Cleaning rdmc_2.9.3
       
  now you should see the two versions side-by-side::

      % ls -d /opt/i3/ports/include/r* 
      /opt/i3/ports/include/rdmc-2.9.3/  /opt/i3/ports/include/rdmc-2.9.5/

  and the older code should happily detect the correct rdmc.

         
