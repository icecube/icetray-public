cmake_minimum_required(VERSION 2.8)

project(test_unregistered)

##################################################
# This project depends on python.
# Need to find the header files and libraries.
##################################################
find_package(PythonInterp)
find_package(PythonLibs)
include_directories(${PYTHON_INCLUDE_PATH})

# Be nice and don't add the default 'lib' at
# the beginning of libraries that are going
# to be imported into python.
set(CMAKE_SHARED_LIBRARY_PREFIX "")

if(APPLE)
  # This is the standard for Linux.
  # On Macs you can't import .dylibs in python
  # So we need to set this manually.
  set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif()

add_library(test_unregistered SHARED
  unregistered_track.cxx
)

##################################################
# This project depends on boost::python and 
# boost::serialization.
# Need to find the header files and libraries.
##################################################
set(BOOST_COMPONENTS python serialization)
find_package(Boost REQUIRED COMPONENTS 
  ${BOOST_COMPONENTS} )

##################################################
# We should have everything we need at this point
# from the dependent projects.  Start setting up
# the build.
##################################################

include_directories(${Boost_INCLUDE_DIRS})

set(I3_LIBRARIES icetray dataclasses)

set(LINK_LIBRARIES ${Boost_LIBRARIES})
list(APPEND LINK_LIBRARIES ${PYTHON_LIBRARIES})

##################################################
# Link to the IceCube libraries.  On Macs they're
# dylibs, so we need to set the suffux accordingly
##################################################

if(APPLE)
  set(LINK_LIB_SUFFIX ".dylib")
else()
  set(LINK_LIB_SUFFIX ".so")
endif()
foreach(I3_LIBRARY ${I3_LIBRARIES})
  list(APPEND LINK_LIBRARIES ${CMAKE_BINARY_DIR}/lib/lib${I3_LIBRARY}${LINK_LIB_SUFFIX})
endforeach()

target_link_libraries(test_unregistered ${LINK_LIBRARIES})
