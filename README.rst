About
-----

This repository contains the public portions of the analysis software developed by the IceCube collaboration. It includes the IO libraries, core APIs, and event viewer (steamshovel) and is intended primarily as a dependency for other, more interesting code. For more detail on the intended processing architecture, please see the Quick Start chapter of the documentation, located at `docs/info/quickstart.rst`.

Except where otherwise noted, all files are distributed under the 2-clause BSD license. Acknowledgments and patches are appreciated; please submit patches using GitHub pull requests.

Documentation
-------------

The main documentation for the software is in the docs folder. After building the software, you can build a pretty, searchable copy by running ``make docs``.

Dependencies
------------

This depends on Boost, cmake, and GSL, as well as the usual Python packages. You also need a C++11 compiler. This software is designed to run and work on a variety of operating systems (all Linuxes, Mac OS X, and FreeBSD) and architectures (at least 64-bit x86 and POWER).

Minimum versions:

	- A C++11 compiler
	- Boost >= 1.48
	- cmake >= 3.1
	- Python >= 3.4
  
On Ubuntu/Debian, you can install these dependencies by doing:

.. code-block:: shell

	apt-get install build-essential cmake libbz2-dev libgsl0-dev libcfitsio-dev libboost-system-dev libboost-thread-dev libboost-date-time-dev libboost-python-dev libboost-serialization-dev libboost-filesystem-dev libboost-program-options-dev libboost-regex-dev libboost-iostreams-dev python3-numpy fftw3-dev libboost-python-numpy
	
If your system defaults to Python 2, please do the following:

	1. Install Python 3 *from the system package manager*
	2. Make sure the python-3 version of the Boost library is installed (on Ubuntu, this is part of the standard boost-python package referenced above)
	3. When you run cmake below, pass ``-DPython_EXECUTABLE=`which python3```


How to Build
------------

To build:

.. code-block:: shell

	cd icetray-public
	mkdir build
	cd build
	cmake ..
	make


To build the documentation in the build directory type:

.. code-block:: shell

	./env-shell.sh make docs

This will construct an html version of the documentation.  This builds the documentation in the build/docs folder.  Open build/docs/index.html in your favorite web browser.  You should at least read the quick start portion of the documentation before getting started.
