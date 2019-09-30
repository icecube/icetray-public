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
set(Boost_NO_BOOST_CMAKE ON)

if(NOT DEFINED Boost_PYTHON_TYPE)
        set(Boost_PYTHON_TYPE python)

	# Detect attempts to be clever with the naming of the Boost Python library
	string(REGEX REPLACE ".*libpython([0-9])\\.[0-9]+.*\\..*" "\\1" PYTHONMAJORVER ${PYTHON_LIBRARIES})
	string(REGEX REPLACE ".*libpython[0-9]\\.([0-9]+).*\\..*" "\\1" PYTHONMINORVER ${PYTHON_LIBRARIES})

	# Hack for some old Boost CMake modules
	set(_Boost_PYTHON${PYTHONMAJORVER}_HEADERS "boost/python.hpp")
	set(_Boost_PYTHON${PYTHONMAJORVER}${PYTHONMINORVER}_HEADERS "boost/python.hpp")

	find_package(Boost QUIET COMPONENTS python${PYTHONMAJORVER}${PYTHONMINORVER})
	if (${Boost_PYTHON${PYTHONMAJORVER}${PYTHONMINORVER}_FOUND})
		set(Boost_PYTHON_TYPE python${PYTHONMAJORVER}${PYTHONMINORVER})
	else()
		find_package(Boost QUIET COMPONENTS python${PYTHONMAJORVER})
		if (${Boost_PYTHON${PYTHONMAJORVER}_FOUND})
			set(Boost_PYTHON_TYPE python${PYTHONMAJORVER})
		endif()
	endif()
endif()

find_package(Boost COMPONENTS system thread date_time filesystem program_options regex iostreams ${Boost_PYTHON_TYPE} REQUIRED)

if(Boost_FOUND)
	set(BOOST_FOUND TRUE CACHE BOOL "Boost found successfully")
	set(BOOST_INCLUDE_DIR ${Boost_INCLUDE_DIR} CACHE PATH "Path to the boost include directories.")
	set(BOOST_LIBRARIES ${Boost_LIBRARIES} CACHE PATH "Boost libraries")
	include_directories(${CMAKE_SOURCE_DIR}/cmake/tool-patches/boost-new)

	# XXX: following obsolete?
	foreach(lib ${BOOST_LIBRARIES})
		if(NOT ${lib} STREQUAL "optimized" AND NOT ${lib} STREQUAL "debug")
			add_custom_command(TARGET install_tool_libs
			    PRE_BUILD
			    COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/lib/tools
			    COMMAND ${CMAKE_SOURCE_DIR}/cmake/install_shlib.py ${lib} ${CMAKE_INSTALL_PREFIX}/lib/tools)
		endif(NOT ${lib} STREQUAL "optimized" AND NOT ${lib} STREQUAL "debug")
	endforeach(lib ${BOOST_LIBRARIES})
endif(Boost_FOUND)

