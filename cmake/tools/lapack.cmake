## If you're having trouble with detection on a debian/ubuntu
## multiarch system (Ubuntu 11.04+), uncomment the lines around the
## find_package() call.

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
  colormsg(YELLOW "*** GotoBLAS2 not found in \$I3_PORTS")
  colormsg(YELLOW "*** Searching for 3rd party LAPACK (I hope you linked it against SuiteSparse)")

  set(BLA_STATIC FALSE)
  #set(CMAKE_REQUIRED_FLAGS "-L/usr/lib/i386-linux-gnu -L/usr/lib/x86_64-linux-gnu -lgfortran")
  find_package(LAPACK)
  #unset(CMAKE_REQUIRED_FLAGS)
endif(LAPACK_FOUND)

## uncomment the following for debugging...
#colormsg("BLAS_LIBRARIES: ${BLAS_LIBRARIES}")
#colormsg("LAPACK_LIBRARIES: ${LAPACK_LIBRARIES}")
