colormsg("")
colormsg(HICYAN "libarchive")

find_package(PkgConfig)
if(${CMAKE_VERSION} VERSION_LESS "2.8.2")
  pkg_check_modules(PC_LA libarchive)
else(${CMAKE_VERSION} VERSION_LESS "2.8.2")
  pkg_check_modules(PC_LA QUIET libarchive)
endif(${CMAKE_VERSION} VERSION_LESS "2.8.2")

## lzma is optional for libarchive, but we don't have a clean way to
## check if it's linked in or not. so, let's look for it then include it

find_library(LZMA_LIBRARIES lzma
             HINTS ${I3_PORTS}/lib
                   ${PC_LA_LIBDIR}
                   ${PC_LA_LIBRARY_DIRS}
                   ${TOOL_SYSTEM_PATH})

find_path(LZMA_INCLUDE_DIR lzma.h
          HINTS ${I3_PORTS}/include
                ${PC_LA_INCLUDEDIR}
                ${PC_LA_INCLUDE_DIRS})

find_library(LIBARCHIVE_LIBRARIES archive
             HINTS ${I3_PORTS}/lib
                   ${PC_LA_LIBDIR}
                   ${PC_LA_LIBRARY_DIRS}
                   ${TOOL_SYSTEM_PATH})

find_path(LIBARCHIVE_INCLUDE_DIR archive.h
          HINTS ${I3_PORTS}/include
                ${PC_LA_INCLUDEDIR}
                ${PC_LA_INCLUDE_DIRS})

if(NOT ${LIBARCHIVE_INCLUDE_DIR} MATCHES "-NOTFOUND")
  found_ok("archive.h found at ${LIBARCHIVE_INCLUDE_DIR}")
endif(NOT ${LIBARCHIVE_INCLUDE_DIR} MATCHES "-NOTFOUND")
if(NOT ${LIBARCHIVE_LIBRARIES} MATCHES "-NOTFOUND")
  found_ok(${LIBARCHIVE_LIBRARIES})
endif(NOT ${LIBARCHIVE_LIBRARIES} MATCHES "-NOTFOUND")

if(LIBARCHIVE_INCLUDE_DIR AND LIBARCHIVE_LIBRARIES)
  set(LIBARCHIVE_FOUND TRUE CACHE BOOL "")
  add_custom_command(TARGET install_tool_libs
    PRE_BUILD
    COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/lib/tools
    COMMAND ${CMAKE_SOURCE_DIR}/cmake/install_shlib.py ${LZMA_LIBRARIES} ${CMAKE_INSTALL_PREFIX}/lib/tools
    COMMAND ${CMAKE_SOURCE_DIR}/cmake/install_shlib.py ${LIBARCHIVE_LIBRARIES} ${CMAKE_INSTALL_PREFIX}/lib/tools)
endif(LIBARCHIVE_INCLUDE_DIR AND LIBARCHIVE_LIBRARIES)
