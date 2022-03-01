# Copyright (c) 2015-2019 IceCube Collaboration
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.

# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
#  $Id$
#  

colormsg("")
colormsg(HICYAN "Boost")
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

## cmake is dropping support for its FindBoost.cmake, relying instead
## on Boost's BoostConfig.cmake (available boost >= v1.70). when the
## time comes (and our reliance on boost < 1.70 (ie cvmfs) has waned),
## remove the following line. see #2585
set(Boost_NO_BOOST_CMAKE ON)

if(NOT DEFINED Boost_PYTHON_TYPE)
	set(Boost_PYTHON_TYPE python)

	# Hack for some old Boost CMake modules
	set(_Boost_PYTHON${Python_VERSION_MAJOR}_HEADERS "boost/python.hpp")
	set(_Boost_PYTHON${Python_VERSION_MAJOR}${Python_VERSION_MINOR}_HEADERS "boost/python.hpp")

	find_package(Boost QUIET COMPONENTS python${Python_VERSION_MAJOR}${Python_VERSION_MINOR})
	if (${Boost_PYTHON${Python_VERSION_MAJOR}${Python_VERSION_MINOR}_FOUND})
		set(Boost_PYTHON_TYPE python${Python_VERSION_MAJOR}${Python_VERSION_MINOR})
	else()
		find_package(Boost QUIET COMPONENTS python${Python_VERSION_MAJOR})
		if (${Boost_PYTHON${Python_VERSION_MAJOR}_FOUND})
			set(Boost_PYTHON_TYPE python${Python_VERSION_MAJOR})
		endif()
	endif()
endif()

find_package(Boost COMPONENTS system thread date_time filesystem program_options regex iostreams ${Boost_PYTHON_TYPE} REQUIRED)

if(Boost_FOUND)
	set(BOOST_FOUND TRUE CACHE BOOL "Boost found successfully")
	set(BOOST_INCLUDE_DIR ${Boost_INCLUDE_DIR} CACHE PATH "Path to the boost include directories.")
	set(BOOST_LIBRARIES ${Boost_LIBRARIES} CACHE PATH "Boost libraries")
	include_directories(${CMAKE_SOURCE_DIR}/cmake/tool-patches/boost-new)
endif(Boost_FOUND)
