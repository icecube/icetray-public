## If you're having trouble with detection on a debian/ubuntu
## multiarch system (Ubuntu 11.04+), uncomment the lines around the
## find_package() call.

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
  ## Assume GotoBLAS was built in ports w/ gfortran
  list(APPEND BLAS_LIBRARIES "-lgfortran")
else(BLAS_FOUND)
  unset(BLAS_CONFIG_ERROR)
  unset(BLAS_INCLUDE_DIR)
  colormsg(YELLOW "*** GotoBLAS2 not found in \$I3_PORTS")
  colormsg(YELLOW "*** Searching for 3rd party BLAS (I hope you linked it against SuiteSparse)")

  set(BLA_STATIC FALSE)
  #set(CMAKE_REQUIRED_FLAGS "-L/usr/lib/i386-linux-gnu -L/usr/lib/x86_64-linux-gnu -lgfortran")
  find_package(BLAS)
  #unset(CMAKE_REQUIRED_FLAGS)
endif(BLAS_FOUND)

else(NOT APPLE)
  colormsg("")
  colormsg(HICYAN "blas")
  find_package(BLAS)
endif(NOT APPLE)
