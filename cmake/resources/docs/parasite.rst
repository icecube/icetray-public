.. index:: parasite

Parasitic metaprojects
======================

.. highlight:: sh

It is sometimes convenient to add projects to a preexisting, read-only
metaproject, for example, to add a private analysis project to an IceRec
release distributed via CVMFS. The cmake project includes a second, "parasitic"
build system for this purpose. To create a parasitic metaproject, start by
creating and empty source directory and adding the cmake project::

	> mkdir src && cd src
	> svn co http://code.icecube.wisc.edu/svn/meta-projects/combo/stable/cmake

Then, add a CMakeLists.txt file with the following contents::

	cmake_minimum_required(VERSION 2.6.4 FATAL_ERROR)
	include(cmake/toplevel-parasite.cmake NO_POLICY_SCOPE)

Now, make a build directory and run cmake::

	> mkdir ../build && cd ../build
	> cmake ../src -DMETAPROJECT=/path/to/build/directory -DCMAKE_INSTALL_PREFIX=icerec-plus.${OS_ARCH}
	-- The C compiler identification is AppleClang 6.0.0.6000054
	-- The CXX compiler identification is AppleClang 6.0.0.6000054
	...
	-- 
	-- Configuring parasitic metaproject 
	-- 
	-- Base metaproject: /Users/jakob/Documents/IceCube/metaprojects/icerec-lite/build
	-- Built from:       http://code.icecube.wisc.edu/svn/sandbox/jvansanten/metaprojects/icerec-lite/trunk r134422
	-- Install prefix:   /Users/jakob/Documents/IceCube/metaprojects/parasite/build/icerec-plus.osx_10_x86_64
	...
	-- Configuring parasitic projects: 
	-- 
	-- Syminking resources from host projects
	-- Syminking Python packages from /Users/jakob/Documents/IceCube/metaprojects/icerec-lite/build/lib/icecube/
	-- Symlinking binaries from /Users/jakob/Documents/IceCube/metaprojects/icerec-lite/build/bin/
	-- Symlinking libraries from /Users/jakob/Documents/IceCube/metaprojects/icerec-lite/build/lib/
	-- Configuring done
	-- Generating done
	-- Build files have been written to: /Users/jakob/Documents/IceCube/metaprojects/parasite/build

The argument to ``-DMETAPROJECT`` can specify a pre-build metaproject
distributed via CVMFS, e.g. ``icerec/V04-11-10``, or the full path to an
existing build directory.

At this point, you have a metaproject that consists entirely of symbolic links
to the parent metaproject. A tarball created from the metaproject in this stage
is extremely small::

	> make tarball
	Scanning dependencies of target tarball-install
	[ 50%] Tarball install
	Install the project...
	-- Install configuration: ""
	-- Installing: /Users/jakob/Documents/IceCube/metaprojects/parasite/build/icerec-plus./lib/icecube/__init__.py
	-- Installing: /Users/jakob/Documents/IceCube/metaprojects/parasite/build/icerec-plus./lib/icecube/load_pybindings.py
	-- Installing: /Users/jakob/Documents/IceCube/metaprojects/parasite/build/icerec-plus././env-shell.sh
	Built target tarball-install
	Scanning dependencies of target tarball-finish
	Finishing tarball of /Users/jakob/Documents/IceCube/metaprojects/parasite/build/icerec-plus.osx_10_x86_64
	Creating /Users/jakob/Documents/IceCube/metaprojects/parasite/build/icerec-plus.osx_10_x86_64.tar.gz
	tar: Removing leading '/' from member names
	Checksumming /Users/jakob/Documents/IceCube/metaprojects/parasite/build/icerec-plus.osx_10_x86_64.tar.gz
	Built target tarball-finish
	Scanning dependencies of target tarball
	Built target tarball
	> ls -lh icerec-plus.osx_10_x86_64.tar.gz
	-rw-r--r--  1 jakob  staff   4.1K Aug 12 21:48 icerec-plus.osx_10_x86_64.tar.gz

Now you can add extra projects, for example, simclasses::

	> svn co http://code.icecube.wisc.edu/svn/projects/simclasses/trunk ../src/simclasses
	> make rebuild_cache
	...
	-- Configuring parasitic projects: 
	-- 
	-- + simclasses
	-- +-- python [symlinks] 
	-- +-- simclasses-pybindings 
	-- Syminking resources from host projects
	...
	> make

This metaproject will contain simclasses, linked against libraries in the
parent metaproject, plus symbolic links to every other project in the parent
metaproject. Since the symbolic links take up virtually no disk space, the
tarball of this metaproject is quite small.


