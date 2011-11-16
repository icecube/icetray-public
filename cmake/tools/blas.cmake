## If you're having trouble with detection on a debian/ubuntu
## multiarch system (Ubuntu 11.04+), uncomment the lines around the
## find_package() call.

if(META_PROJECT MATCHES "^icerec")

tooldef(blas
  include
  cblas.h
  lib
  NONE
  goto2
)

if(BLAS_FOUND)
  set(BLASINI3PORTS_FOUND TRUE)
else(BLAS_FOUND)
  colormsg(YELLOW "*** GotoBLAS2 not found in \$I3_PORTS")
  colormsg(YELLOW "*** Searching for 3rd party BLAS (I hope you linked it against SuiteSparse)")

  set(BLA_STATIC FALSE)
  #set(CMAKE_REQUIRED_FLAGS "-L/usr/lib/i386-linux-gnu -L/usr/lib/x86_64-linux-gnu -lgfortran")
  find_package(BLAS)
  #unset(CMAKE_REQUIRED_FLAGS)
endif(BLAS_FOUND)

endif(META_PROJECT MATCHES "^icerec")
