# - Try to find OpenCL
# This module tries to find an OpenCL implementation on your system. It supports
# AMD / ATI, Apple and NVIDIA implementations, but should work, too.
#
# Once done this will define
#  OPENCL_FOUND        - system has OpenCL
#  OPENCL_INCLUDE_DIRS  - the OpenCL include directory
#  OPENCL_LIBRARIES    - link these to use OpenCL

# we don't use the tooldef() macro, so we have to fudge pretty-printing
colormsg("")
colormsg(HICYAN "OpenCL")
    
IF (APPLE)
  if(CMAKE_SYSTEM_VERSION VERSION_LESS "11.0.0")
    FOUND_NOT_OK("The OpenCL provided by this version of OS X is too old. Disabling.")
  else(CMAKE_SYSTEM_VERSION VERSION_LESS "11.0.0")

    FOUND_OK("Using the OpenCL Framework because we're on Apple")

    # Search for the framework on Apple systems.
    # (cribbed from Modules/FindPython.cmake)
    INCLUDE(CMakeFindFrameworks)
    CMAKE_FIND_FRAMEWORKS(OpenCL)
        
    if(OpenCL_FRAMEWORKS)
        
        SET(OPENCL_FRAMEWORK_INCLUDES)
        FOREACH(dir ${OpenCL_FRAMEWORKS})
            SET(OPENCL_FRAMEWORK_INCLUDES ${OPENCL_FRAMEWORK_INCLUDES} ${dir}/Versions/Current/Headers)
        ENDFOREACH(dir)

        FIND_PATH(OPENCL_INCLUDE_DIR
            NAMES cl.h
            PATHS ${OPENCL_FRAMEWORK_INCLUDES}
        )
        
        REPORT_FIND(OpenCL cl.h ${OPENCL_INCLUDE_DIR})

        # If a framework has been selected for the include path,
        # make sure "-framework" is used to link it.
        IF("${OPENCL_INCLUDE_DIR}" MATCHES "OpenCL\\.framework")
          UNSET(OPENCL_LIBRARIES)
        ENDIF("${OPENCL_INCLUDE_DIR}" MATCHES "OpenCL\\.framework")
        IF(NOT OPENCL_LIBRARIES)
          SET (OPENCL_LIBRARIES "-framework OpenCL" CACHE FILEPATH "OpenCL Framework")
        ENDIF(NOT OPENCL_LIBRARIES)

        set(OPENCL_FOUND TRUE CACHE BOOL "Tool 'opencl' found successfully")

        REPORT_FIND(OpenCL "OpenCL framework" ${OPENCL_LIBRARIES})

    endif(OpenCL_FRAMEWORKS)

  endif(CMAKE_SYSTEM_VERSION VERSION_LESS "11.0.0")
ELSE (APPLE)
    # Unix style platforms
    FIND_LIBRARY(OPENCL_LIBRARIES OpenCL
      HINTS ENV LD_RUN_PATH ENV LD_LIBRARY_PATH
    )

    IF (OPENCL_LIBRARIES)      
        GET_FILENAME_COMPONENT(OPENCL_LIB_DIR ${OPENCL_LIBRARIES} PATH)
        GET_FILENAME_COMPONENT(_OPENCL_INC_CAND ${OPENCL_LIB_DIR}/../../include ABSOLUTE)
        GET_FILENAME_COMPONENT(_OPENCL_INC_CAND_2 ${OPENCL_LIB_DIR}/../include ABSOLUTE)

        # find it on ubuntu systems (package nvidia-current-dev)
        SET(_OPENCL_INC_CAND ${_OPENCL_INC_CAND} ${_OPENCL_INC_CAND_2} /usr/include /usr/include/nvidia-current /opt/AMDAPP/include)

        # The AMD SDK currently does not place its headers
        # in /usr/include, therefore also search relative
        # to the library
        FIND_PATH(OPENCL_INCLUDE_DIRS CL/cl.h PATHS ${_OPENCL_INC_CAND} "/usr/local/cuda/include")
        FIND_PATH(_OPENCL_CPP_INCLUDE_DIRS CL/cl.hpp PATHS ${_OPENCL_INC_CAND} "/usr/local/cuda/include")

        tooldef(opencl
          ${OPENCL_INCLUDE_DIRS}
          CL/cl.h
          ${OPENCL_LIB_DIR}
          NONE
          OpenCL
          )
    ELSE (OPENCL_LIBRARIES)
      # find_package works just fine on some systems
      # where find_library fails, for reasons that aren't
      # entirely clear.  so if find_library fails, try find_package.
      find_package(OpenCL)
      set(OPENCL_FOUND ${OpenCL_FOUND})
      if(OpenCL_FOUND)
	get_filename_component(OPENCL_LIB_DIR ${OpenCL_LIBRARIES} PATH)
	tooldef(opencl
          ${OpenCL_INCLUDE_DIRS}
          CL/cl.h
          ${OPENCL_LIB_DIR}
          NONE
          OpenCL
          )
      endif()
    ENDIF (OPENCL_LIBRARIES)

ENDIF (APPLE)

#=============================================================================
# Copyright 2003-2011 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
IF (OPENCL_FOUND)
  include(CheckSymbolExists)
  IF (CMAKE_VERSION VERSION_GREATER "2.8.5")
    include(CMakePushCheckState)
    CMAKE_PUSH_CHECK_STATE()
  ENDIF (CMAKE_VERSION VERSION_GREATER "2.8.5")
  foreach(VERSION "2_0" "1_2" "1_1" "1_0")
    set(CMAKE_REQUIRED_INCLUDES "${OPENCL_INCLUDE_DIR}")

    if(APPLE)
      CHECK_SYMBOL_EXISTS(
        CL_VERSION_${VERSION}
        "${OPENCL_INCLUDE_DIR}/cl.h"
        OPENCL_VERSION_${VERSION})
    else()
      CHECK_SYMBOL_EXISTS(
        CL_VERSION_${VERSION}
        "${OPENCL_INCLUDE_DIR}/CL/cl.h"
        OPENCL_VERSION_${VERSION})
    endif()

    if(OPENCL_VERSION_${VERSION})
      string(REPLACE "_" "." VERSION "${VERSION}")
      set(OPENCL_VERSION_STRING ${VERSION} CACHE STRING "OpenCL version")
      string(REGEX MATCHALL "[0-9]+" version_components "${VERSION}")
      list(GET version_components 0 major_version)
      list(GET version_components 1 minor_version)
      set(OPENCL_VERSION_MAJOR ${major_version} CACHE INTERNAL "OpenCL major version number")
      set(OPENCL_VERSION_MINOR ${minor_version} CACHE INTERNAL "OpenCL minor version number")
      break()
    endif()
  endforeach()
  IF (CMAKE_VERSION VERSION_GREATER "2.8.5")
    CMAKE_POP_CHECK_STATE()
  ENDIF (CMAKE_VERSION VERSION_GREATER "2.8.5")
ENDIF (OPENCL_FOUND)
