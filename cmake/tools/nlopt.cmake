#  Copyright (C) 2016 the IceCube Collaboration <http://www.icecube.wisc.edu>
#  SPDX-License-Identifier: BSD-2-Clause

# nlopt has first class CMake support, but debian/ubuntu split the C++ bits
# into its own package. while ``find_package(NLopt)`` finds everything, the
# resultant target is ``NLopt::nlopt_cxx`` on these systems.

find_package(NLopt QUIET)
if(NLopt_FOUND)
  set(NLOPT_FOUND TRUE CACHE BOOL "Tool 'nlopt' found successfully" FORCE)
  # we don't use the tooldef() macro, so we have to fudge pretty-printing
  colormsg("")
  colormsg(HICYAN "nlopt")
  message(STATUS "+  version: ${NLOPT_VERSION}")
  message(STATUS "+ includes: ${NLOPT_INCLUDE_DIRS}")
  message(STATUS "+     libs: ${NLOPT_LIBRARY_DIRS}")
else()
TOOLDEF (nlopt
  include
  nlopt.hpp
  libs
  NONE
  nlopt
  )
endif()
