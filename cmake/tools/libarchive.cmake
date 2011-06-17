colormsg("")
colormsg(HICYAN "libarchive")

find_package(PkgConfig)
pkg_check_modules(PC_LA QUIET libarchive)

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
  found_ok("archive.h in ${LIBARCHIVE_INCLUDE_DIR}")
endif(NOT ${LIBARCHIVE_INCLUDE_DIR} MATCHES "-NOTFOUND")
if(NOT ${LIBARCHIVE_LIBRARIES} MATCHES "-NOTFOUND")
  found_ok(${LIBARCHIVE_LIBRARIES})
endif(NOT ${LIBARCHIVE_LIBRARIES} MATCHES "-NOTFOUND")

if(LIBARCHIVE_INCLUDE_DIR AND LIBARCHIVE_LIBRARIES)
  set(LIBARCHIVE_FOUND TRUE CACHE BOOL "")
endif(LIBARCHIVE_INCLUDE_DIR AND LIBARCHIVE_LIBRARIES)
