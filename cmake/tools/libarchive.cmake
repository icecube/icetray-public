colormsg("")
colormsg(HICYAN "libarchive")

find_package(PkgConfig QUIET)
pkg_check_modules(PC_LA QUIET libarchive)

## lzma is optional for libarchive, but we don't have a clean way to
## check if it's linked in or not. so, let's look for it then include it
pkg_check_modules(PC_LZMA QUIET liblzma)

find_library(LZMA_LIBRARIES lzma
             HINTS ${PC_LZMA_LIBDIR}
                   ${PC_LZMA_LIBRARY_DIRS}
		   /usr/local)

find_path(LZMA_INCLUDE_DIR lzma.h
          HINTS ${PC_LZMA_INCLUDEDIR}
                ${PC_LZMA_INCLUDE_DIRS}
		/usr/local)

find_library(LZMA_LIBRARIES lzma
             HINTS ${PC_LA_LIBDIR}                   
                   ${PC_LA_LIBRARY_DIRS}
                   ${TOOL_SYSTEM_PATH})

find_path(LZMA_INCLUDE_DIR lzma.h
          HINTS ${PC_LA_INCLUDEDIR}                
                ${PC_LA_INCLUDE_DIRS})

## look in Homebrew for libarchive
if(APPLE)
  find_library(LIBARCHIVE_LIBRARIES archive
    PATHS /usr/local/opt/libarchive
          /usr/local/opt/libarchive/lib
    NO_DEFAULT_PATH)
  find_path(LIBARCHIVE_INCLUDE_DIR archive.h
    PATHS /usr/local/opt/libarchive
          /usr/local/opt/libarchive/include
    NO_DEFAULT_PATH)
endif()

## give cmake a chance to find a custom install of libarchive
find_library(LIBARCHIVE_LIBRARIES archive
             HINTS ${PC_LA_LIBDIR}                   
                   ${PC_LA_LIBRARY_DIRS}
                   ${TOOL_SYSTEM_PATH}
                   NO_DEFAULT_PATH)
## if that fails, allow searches in CMAKE_PREFIX/lib
find_library(LIBARCHIVE_LIBRARIES archive
             HINTS ${PC_LA_LIBDIR}                   
                   ${PC_LA_LIBRARY_DIRS}
                   ${TOOL_SYSTEM_PATH}
                   )

find_path(LIBARCHIVE_INCLUDE_DIR archive.h
          HINTS ${PC_LA_INCLUDEDIR}                
                ${PC_LA_INCLUDE_DIRS})

if(NOT ${LIBARCHIVE_INCLUDE_DIR} MATCHES "-NOTFOUND")
  found_ok("archive.h found at ${LIBARCHIVE_INCLUDE_DIR}")
endif(NOT ${LIBARCHIVE_INCLUDE_DIR} MATCHES "-NOTFOUND")
if(NOT ${LIBARCHIVE_LIBRARIES} MATCHES "-NOTFOUND")
  found_ok(${LIBARCHIVE_LIBRARIES})
endif(NOT ${LIBARCHIVE_LIBRARIES} MATCHES "-NOTFOUND")

if(LZMA_LIBRARIES)
if(LIBARCHIVE_INCLUDE_DIR AND LIBARCHIVE_LIBRARIES)
  set(LIBARCHIVE_FOUND TRUE CACHE BOOL "")
  add_custom_command(TARGET install_tool_libs
    PRE_BUILD
    COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/lib/tools
    COMMAND ${CMAKE_SOURCE_DIR}/cmake/install_shlib.py ${LZMA_LIBRARIES} ${CMAKE_INSTALL_PREFIX}/lib/tools
    COMMAND ${CMAKE_SOURCE_DIR}/cmake/install_shlib.py ${LIBARCHIVE_LIBRARIES} ${CMAKE_INSTALL_PREFIX}/lib/tools)
endif(LIBARCHIVE_INCLUDE_DIR AND LIBARCHIVE_LIBRARIES)
else()
if(LIBARCHIVE_INCLUDE_DIR AND LIBARCHIVE_LIBRARIES)
  set(LIBARCHIVE_FOUND TRUE CACHE BOOL "")
  add_custom_command(TARGET install_tool_libs
    PRE_BUILD
    COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/lib/tools
    COMMAND ${CMAKE_SOURCE_DIR}/cmake/install_shlib.py ${LIBARCHIVE_LIBRARIES} ${CMAKE_INSTALL_PREFIX}/lib/tools)
endif(LIBARCHIVE_INCLUDE_DIR AND LIBARCHIVE_LIBRARIES)
endif()
