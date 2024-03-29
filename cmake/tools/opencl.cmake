# we don't use the tooldef() macro, so we have to fudge pretty-printing
colormsg("")
colormsg(HICYAN "OpenCL")

option(USE_OPENCL "USE_OPENCL" ON)

# Disable clsim on Apple Silicon until it's ported
if(APPLE AND NOT CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64")
  message(STATUS "+ Disabling OpenCL on Apple Silicon. Support is forthcoming.")
  set (USE_OPENCL OFF)
elseif(NOT USE_OPENCL)
  message(STATUS "+ USE_OPENCL set to false, Disabling OpenCL.")
endif(APPLE AND NOT CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64")

if (USE_OPENCL)
  #Very specific code for cvmfs
  if("$ENV{OPENCL_VENDOR_PATH}" MATCHES "^/cvmfs/icecube.opensciencegrid.org/distrib/")
    get_filename_component(OPENCL_BASE "$ENV{OPENCL_VENDOR_PATH}/../../.." ABSOLUTE)
    message(STATUS "Searching cvmfs ${OPENCL_BASE}")
    if(IS_DIRECTORY ${OPENCL_BASE})
      set(OpenCL_LIBRARY "${OPENCL_BASE}/lib/x86_64/libOpenCL.so")
      set(OpenCL_INCLUDE_DIR "${OPENCL_BASE}/include")
    endif()
  endif()

 find_package(OpenCL QUIET)

 if (OpenCL_FOUND)
    #set uppercase name so macros in project.cmake know what is happening
    SET(OPENCL_FOUND TRUE)
    SET(OPENCL_INCLUDE_DIRS "${OpenCL_INCLUDE_DIRS}")
    get_filename_component(_hint "${OpenCL_LIBRARIES}" DIRECTORY)
    FIND_LIBRARY(OPENCL_LIBRARIES OpenCL HINTS "${_hint}" PATH_SUFFIXES x86_64 lib64)

    message(STATUS "+    version: ${OpenCL_VERSION_STRING}")
    message(STATUS "+    include: ${OPENCL_INCLUDE_DIRS}")
    message(STATUS "+  libraries: ${OPENCL_LIBRARIES}")
  else(OpenCL_FOUND)
    message(STATUS "-  Not Found")
  endif(OpenCL_FOUND)
endif (USE_OPENCL)
