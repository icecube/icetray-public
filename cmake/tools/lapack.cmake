## If you're having trouble with detection on a debian/ubuntu
## multiarch system (Ubuntu 11.04+), uncomment the lines around the
## find_package() call.

if(NOT APPLE)
tooldef(lapack
  NONE
  NONE
  lib
  NONE
  goto2
)

if(LAPACK_FOUND)
  set(LAPACKINI3PORTS_FOUND TRUE)
else()
  unset(LAPACK_CONFIG_ERROR)
  unset(LAPACK_INCLUDE_DIR)
  
  find_package(OpenBLAS QUIET)
  if(OpenBLAS_FOUND)
    colormsg("+ OpenBLAS")    
    colormsg("+  version: ${OpenBLAS_VERSION}")
    colormsg("+ includes: ${OpenBLAS_INCLUDE_DIRS}")
    colormsg("+     libs: ${OpenBLAS_LIBRARIES}")
    set(LAPACK_FOUND TRUE CACHE BOOL "Tool LAPACK found successfully" FORCE)
    set(LAPACK_INCLUDE_DIR ${OpenBLAS_INCLUDE_DIRS} CACHE PATH "Headers for OpenBLAS" FORCE)
    set(LAPACK_LIBRARIES ${OpenBLAS_LIBRARIES} CACHE PATH "Libraries for OpenBLAS" FORCE)
  else()
    colormsg(YELLOW "*** GotoBLAS2 not found.")
    colormsg(YELLOW "*** Searching for 3rd party LAPACK (I hope you linked it against SuiteSparse)")

    set(BLAS_STATIC FALSE)
    find_package(LAPACK)
    unset(CMAKE_REQUIRED_FLAGS)
  endif()
endif()

else(NOT APPLE)
  colormsg("")
  colormsg(HICYAN "lapack")
  find_package(LAPACK)
endif()

## uncomment the following for debugging...
#colormsg("BLAS_LIBRARIES: ${BLAS_LIBRARIES}")
#colormsg("LAPACK_LIBRARIES: ${LAPACK_LIBRARIES}")
