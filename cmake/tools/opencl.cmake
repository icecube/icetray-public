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
          SET(OPENCL_LIBRARIES "")
        ENDIF("${OPENCL_INCLUDE_DIR}" MATCHES "OpenCL\\.framework")
        IF(NOT OPENCL_LIBRARIES)
          SET (OPENCL_LIBRARIES "-framework OpenCL" CACHE FILEPATH "OpenCL Framework" FORCE)
        ENDIF(NOT OPENCL_LIBRARIES)

        set(OPENCL_FOUND TRUE CACHE BOOL "Tool 'opencl' found successfully" FORCE)

        REPORT_FIND(OpenCL "OpenCL framework" ${OPENCL_LIBRARIES})

    endif(OpenCL_FRAMEWORKS)

  endif(CMAKE_SYSTEM_VERSION VERSION_LESS "11.0.0")
ELSE (APPLE)
    # Unix style platforms
    FIND_LIBRARY(OPENCL_LIBRARIES OpenCL
      HINTS ENV LD_LIBRARY_PATH
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
    ENDIF (OPENCL_LIBRARIES)

ENDIF (APPLE)
