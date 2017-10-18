## -*- mode: cmake; mode: folding; folded-file: t -*-
## $Id$

## This is the tool detection file for BLAS and LAPACK
##
## Historically we used our `tooldef()` macro to find GotoBLAS, and
## that was enough for our needs. Unfortunately, GotoBLAS is now
## abandonware, but OpenBLAS has taken up the torch and is our
## preferred BLAS implementation. Properly built and installed (as in
## our CVMFS repo), it provides the fastest available BLAS
## implementation. ATLAS is currently our second choice, as it also
## provides a highly optimized implementation. Apple's Accelerate
## Framework is our first choice on macOS, as it is always available
## and easily detected. The organization of this file reflects the
## choices.
##
## Detection routines for individual platforms should be in distinct
## blocks. Said blocks should be commented at the beginning describing
## the intent of the block. Anything "clever" in the block should be
## commented describing the intent of said "clever" code. CMake's
## `return()` macro should be used to escape from this file, at the
## successful completion of a block, and fall through to the next
## block if unsuccessful. This "style" is encouraged to maintain
## clarity, as CMake's conditionals can get hairy.
##
## For every platform and BLAS, exports from this file should be:
##
## 	BLAS_FOUND		- True if your BLAS was found
## 	BLAS_LIBRARIES		- The libraries that are part of your BLAS
## 	BLAS_INCLUDE_DIR	- Where the include files of your BLAS are located
##
## 	LAPACK_FOUND		- True if your LAPACK was found
## 	LAPACK_LIBRARIES	- The libraries that are part of your LAPACK
## 	LAPACK_INCLUDE_DIR	- Where the include files of your LAPACK are located
##
## Further notes:
## LAPACK is often integrated into the OpenBLAS library. To get this
## on Arch Linux, use `openblas-lapack` from AUR.
##

colormsg("")
colormsg(CYAN "blas and lapack")

# {{{ cmake easily finds Accelerate Framework

if(APPLE)
  set(BLA_VENDOR "Apple")
  #find_package(BLAS)
  find_package(LAPACK)  ## this call also searches for BLAS
  return()
endif()

# }}}

# {{{ search for OpenBLAS in IceCube's CVMFS

set(BLA_VENDOR "OpenBLAS")
message(STATUS "Searching for OpenBLAS in IceCube's CVMFS (or Arch's AUR)")
find_package(${BLA_VENDOR} QUIET)
if(OpenBLAS_FOUND)
  set(BLAS_FOUND TRUE CACHE BOOL "Tool BLAS found successfully" FORCE)
  set(BLAS_INCLUDE_DIR ${OpenBLAS_INCLUDE_DIRS} CACHE PATH "Headers for OpenBLAS" FORCE)
  set(BLAS_LIBRARIES ${OpenBLAS_LIBRARIES} CACHE PATH "Libraries for OpenBLAS" FORCE)
  set(LAPACK_FOUND TRUE CACHE BOOL "Tool LAPACK found successfully" FORCE)
  set(LAPACK_INCLUDE_DIR ${OpenBLAS_INCLUDE_DIRS} CACHE PATH "Headers for OpenBLAS" FORCE)
  set(LAPACK_LIBRARIES ${OpenBLAS_LIBRARIES} CACHE PATH "Libraries for OpenBLAS" FORCE)
  found_ok("${BLA_VENDOR}")
  found_ok(" version: ${${BLA_VENDOR}_VERSION}")
  found_ok("includes: ${BLAS_INCLUDE_DIR}")
  found_ok("    libs: ${BLAS_LIBRARIES}")
  found_ok("  lapack: ${LAPACK_LIBRARIES}")
  return()
endif()

# }}}

# {{{ No CVMFS? Ubuntu pkg-config's OpenBLAS
## use `apt-get install libopenblas-dev` to get it

set(BLA_VENDOR "OpenBLAS")
message(STATUS "Searching for ${BLA_VENDOR} via pkg-config")
find_package(PkgConfig QUIET)
pkg_check_modules(PC_${BLA_VENDOR} QUIET blas-openblas)

if(PC_${BLA_VENDOR}_FOUND)
  find_library(${BLA_VENDOR}_LIBRARIES openblas HINTS ${PC_${BLA_VENDOR}_LIBRARY_DIRS})

  find_file(${BLA_VENDOR}_BUILDINFO openblas_config.h HINTS ${PC_${BLA_VENDOR}_INCLUDE_DIRS})
  execute_process(COMMAND grep VERSION ${${BLA_VENDOR}_BUILDINFO}
    COMMAND grep -Eo "[0-9][0-9]?\\.[0-9][0-9]?\\.[0-9][0-9]?"
    OUTPUT_VARIABLE ${BLA_VENDOR}_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  if(PC_${BLA_VENDOR}_INCLUDE_DIRS AND ${BLA_VENDOR}_LIBRARIES)
    set(BLAS_FOUND TRUE CACHE BOOL "Tool BLAS found successfully" FORCE)
    set(BLAS_INCLUDE_DIR ${PC_${BLA_VENDOR}_INCLUDE_DIRS} CACHE PATH "Headers for BLAS" FORCE)
    set(BLAS_LIBRARIES ${${BLA_VENDOR}_LIBRARIES} CACHE PATH "Libraries for BLAS" FORCE)

    pkg_check_modules(PC_LAPACK QUIET lapack-openblas)
    find_library(LAPACK_LIBRARIES lapack HINTS ${PC_LAPACK_LIBRARY_DIRS})
    list(INSERT LAPACK_LIBRARIES 0 ${BLAS_LIBRARIES})
    if(LAPACK_LIBRARIES)
      set(LAPACK_FOUND TRUE CACHE BOOL "Tool LAPACK found successfully" FORCE)
    endif()

    found_ok(${BLA_VENDOR})
    found_ok(" version: ${${BLA_VENDOR}_VERSION}")
    found_ok("includes: ${BLAS_INCLUDE_DIR}")
    found_ok("    libs: ${BLAS_LIBRARIES}")
    found_ok("  lapack: ${LAPACK_LIBRARIES}")
    return()
  endif()
endif()

# }}}

# {{{ Not on Ubuntu? Maybe you have "red hat"'s OpenBLAS?

## use `yum install openblas-devel` to get it

set(BLA_VENDOR "OpenBLAS")
message(STATUS "Searching for ${BLA_VENDOR}")
find_package(BLAS QUIET)
if(BLAS_FOUND)
  set(${BLA_VENDOR}_INCLUDE_DIR "/usr/include/openblas")
  set(BLAS_INCLUDE_DIR ${${BLA_VENDOR}_INCLUDE_DIR})
  find_file(${BLA_VENDOR}_BUILDINFO openblas_config.h PATHS ${${BLA_VENDOR}_INCLUDE_DIR})
  execute_process(COMMAND grep VERSION ${${BLA_VENDOR}_BUILDINFO}
    COMMAND grep -Eo "[0-9][0-9]?\\.[0-9][0-9]?\\.[0-9][0-9]?"
    OUTPUT_VARIABLE ${BLA_VENDOR}_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  find_package(LAPACK QUIET)
  found_ok(${BLA_VENDOR})
  found_ok(" version: ${${BLA_VENDOR}_VERSION}")
  found_ok("includes: ${BLAS_INCLUDE_DIR}")
  found_ok("    libs: ${BLAS_LIBRARIES}")
  found_ok("  lapack: ${LAPACK_LIBRARIES}")
  return()
endif()

# }}}

# {{{ hmm... no OpenBLAS? Let's try ATLAS...

set(BLA_VENDOR "ATLAS")
message(STATUS "Searching for ${BLA_VENDOR} via pkg-config")
find_package(PkgConfig QUIET)
pkg_check_modules(PC_atlas QUIET atlas)

if(PC_${BLA_VENDOR}_FOUND)
  find_library(${BLA_VENDOR}_LIBRARIES tatlas HINTS ${PC_${BLA_VENDOR}_LIBRARY_DIRS})

  find_file(${BLA_VENDOR}_BUILDINFO atlas_buildinfo.h HINTS ${PC_${BLA_VENDOR}_INCLUDE_DIRS})
  execute_process(COMMAND grep ATL_VERS ${${BLA_VENDOR}_BUILDINFO}
    COMMAND grep -Eo "[0-9][0-9]?\\.[0-9][0-9]?\\.[0-9][0-9]?"
    OUTPUT_VARIABLE ${BLA_VENDOR}_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  if(PC_${BLA_VENDOR}_INCLUDE_DIRS AND ${BLA_VENDOR}_LIBRARIES)
    set(BLAS_FOUND TRUE CACHE BOOL "Tool BLAS found successfully" FORCE)
    set(BLAS_INCLUDE_DIR ${PC_${BLA_VENDOR}_INCLUDE_DIRS} CACHE PATH "Headers for BLAS" FORCE)
    set(BLAS_LIBRARIES ${${BLA_VENDOR}_LIBRARIES} CACHE PATH "Libraries for BLAS" FORCE)

    find_package(LAPACK QUIET)	## cmake will possibly override our above BLAS_* settings

    found_ok(${BLA_VENDOR})
    found_ok(" version: ${${BLA_VENDOR}_VERSION}")
    found_ok("includes: ${BLAS_INCLUDE_DIR}")
    found_ok("    libs: ${BLAS_LIBRARIES}")
    found_ok("  lapack: ${LAPACK_LIBRARIES}")
    return()
  endif()
endif()

# }}}

# {{{ a more generic search for ATLAS

set(BLA_VENDOR "ATLAS")
message(STATUS "Searching for ${BLA_VENDOR}")
set(_cfls_old ${CMAKE_FIND_LIBRARY_SUFFIXES})
set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES} .so.3)
find_package(BLAS QUIET)
set(CMAKE_FIND_LIBRARY_SUFFIXES ${_cfls_old})
if(BLAS_FOUND)
  find_package(LAPACK QUIET)
  found_ok(${BLA_VENDOR})
  found_ok(" version: ${${BLA_VENDOR}_VERSION}")
  found_ok("includes: ${BLAS_INCLUDE_DIR}")
  found_ok("    libs: ${BLAS_LIBRARIES}")
  found_ok("  lapack: ${LAPACK_LIBRARIES}")
  return()
endif()

# }}}

# {{{ ok. let's find something, even if it's not optimized

set(BLA_VENDOR "All")
find_package(BLAS)
if(BLAS_FOUND)
  find_package(LAPACK QUIET)
  found_ok(${BLA_VENDOR})
  found_ok(" version: ${${BLA_VENDOR}_VERSION}")
  found_ok("includes: ${BLAS_INCLUDE_DIR}")
  found_ok("    libs: ${BLAS_LIBRARIES}")
  found_ok("  lapack: ${LAPACK_LIBRARIES}")
endif()
# }}}
