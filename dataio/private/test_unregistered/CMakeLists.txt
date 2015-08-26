cmake_minimum_required(VERSION 2.8)

project(test_unregistered)

find_package(PythonInterp)
find_package(PythonLibs)
include_directories(${PYTHON_INCLUDE_PATH})

set(CMAKE_SHARED_LIBRARY_PREFIX "")

# it's unnatural to include the lib at the
# beginning when importing in python
add_library(test_unregistered SHARED
  unregistered_track.cxx
)

set(BOOST_COMPONENTS python serialization)
find_package(Boost REQUIRED COMPONENTS 
  ${BOOST_COMPONENTS} )
include_directories(${Boost_INCLUDE_DIRS})
set(LINK_LIBRARIES ${Boost_LIBRARIES})
if(APPLE)
  list(APPEND LINK_LIBRARIES ${CMAKE_BINARY_DIR}/lib/libicetray.dylib)
  list(APPEND LINK_LIBRARIES ${CMAKE_BINARY_DIR}/lib/libdataclasses.dylib)
else()
  list(APPEND LINK_LIBRARIES ${CMAKE_BINARY_DIR}/lib/libicetray.so)
  list(APPEND LINK_LIBRARIES ${CMAKE_BINARY_DIR}/lib/libdataclasses.so)
endif()

target_link_libraries(test_unregistered ${LINK_LIBRARIES})
