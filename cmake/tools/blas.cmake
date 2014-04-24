## If you're having trouble with detection on a debian/ubuntu
## multiarch system (Ubuntu 11.04+), uncomment the lines around the
## find_package() call.

if (${OSTYPE} STREQUAL "FreeBSD")
  set(CMAKE_REQUIRED_FLAGS "-L/usr/local/lib/gcc47")
  set(BLAS_LINK_FLAGS ${CMAKE_REQUIRED_FLAGS})
endif ()

if(NOT APPLE)
tooldef(blas
  NONE
  NONE
  lib
  NONE
  goto2
)

if(BLAS_FOUND)
  set(BLASINI3PORTS_FOUND TRUE)
else(BLAS_FOUND)
  unset(BLAS_CONFIG_ERROR)
  unset(BLAS_INCLUDE_DIR)
  colormsg(YELLOW "*** GotoBLAS2 not found in \$I3_PORTS")
  colormsg(YELLOW "*** Searching for 3rd party BLAS (I hope you linked it against SuiteSparse)")

  set(BLA_STATIC FALSE)
  #set(CMAKE_REQUIRED_FLAGS "-L/usr/lib/i386-linux-gnu -L/usr/lib/x86_64-linux-gnu -lgfortran")
  find_package(BLAS)
  # make sure we have cblas as well (goto2 always includes it)
  if(BLAS_FOUND)
    set(CMAKE_REQUIRED_LIBRARIES ${BLAS_LIBRARIES})
    check_library_exists(${BLAS_LIBRARIES} "cblas_sgemm" "" _have_cblas)
    if (NOT _have_cblas)
	unset(_have_cblas CACHE)
	find_library(CBLAS_LIBRARY NAMES cblas libcblas)
    	check_library_exists(${CBLAS_LIBRARY} "cblas_sgemm" "" _have_cblas)
    endif()
    if (NOT _have_cblas)
	set(BLAS_FOUND FALSE)
	colormsg(YELLOW "Your BLAS doesn't have a C API")
    else()
	list(APPEND BLAS_LIBRARIES ${CBLAS_LIBRARY})
    endif()
  endif(BLAS_FOUND)
  if(BLAS_FOUND)
    set(BLAS_FOUND TRUE CACHE BOOL "Tool BLAS found successfully" FORCE)
    set(BLAS_LIBRARIES "${BLAS_LIBRARIES}" CACHE PATH "Libraries for tool BLAS" FORCE)
  endif(BLAS_FOUND)
endif(BLAS_FOUND)

else(NOT APPLE)
  colormsg("")
  colormsg(HICYAN "blas")
  find_package(BLAS)
endif(NOT APPLE)

unset(CMAKE_REQUIRED_FLAGS)
unset(CMAKE_REQUIRED_LIBRARIES)
