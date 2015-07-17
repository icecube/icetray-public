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
else(LAPACK_FOUND)
  unset(LAPACK_CONFIG_ERROR)
  unset(LAPACK_INCLUDE_DIR)
  
  find_package(OpenBLAS)
  if(OpenBLAS_FOUND)
    colormsg("+ OpenBLAS")    
    colormsg("+  version: ${OpenBLAS_VERSION}")
    colormsg("+ includes: ${OpenBLAS_INCLUDE_DIRS}")
    colormsg("+     libs: ${OpenBLAS_LIBRARIES}")
    set(LAPACK_FOUND TRUE CACHE BOOL "Tool LAPACK found successfully" FORCE)
    set(LAPACK_INCLUDE_DIR ${OpenBLAS_INCLUDE_DIRS} CACHE PATH "Headers for OpenBLAS" FORCE)
    set(LAPACK_LIBRARIES ${OpenBLAS_LIBRARIES} CACHE PATH "Libraries for OpenBLAS" FORCE)
  else(OpenBLAS_FOUND)
    colormsg(YELLOW "*** GotoBLAS2 not found in \$I3_PORTS")
    colormsg(YELLOW "*** Searching for 3rd party LAPACK (I hope you linked it against SuiteSparse)")

    set(BLAS_STATIC FALSE)
    #set(CMAKE_REQUIRED_FLAGS "-L/usr/lib/i386-linux-gnu -L/usr/lib/x86_64-linux-gnu -lgfortran")
    find_package(LAPACK)
    unset(CMAKE_REQUIRED_FLAGS)
  endif(OpenBLAS_FOUND)
endif(LAPACK_FOUND)

else(NOT APPLE)
  colormsg("")
  colormsg(HICYAN "lapack")
  find_package(LAPACK)
endif(NOT APPLE)

## uncomment the following for debugging...
#colormsg("BLAS_LIBRARIES: ${BLAS_LIBRARIES}")
#colormsg("LAPACK_LIBRARIES: ${LAPACK_LIBRARIES}")
