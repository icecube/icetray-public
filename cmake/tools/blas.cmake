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
else(BLAS_FOUND)
  unset(BLAS_CONFIG_ERROR)
  unset(BLAS_INCLUDE_DIR)
  colormsg(YELLOW "*** GotoBLAS2 not found in \$I3_PORTS")
  colormsg(YELLOW "*** Searching for 3rd party BLAS (I hope you linked it against SuiteSparse)")

  set(BLA_STATIC FALSE)
  #set(CMAKE_REQUIRED_FLAGS "-L/usr/lib/i386-linux-gnu -L/usr/lib/x86_64-linux-gnu -lgfortran")
  if (${OSTYPE} STREQUAL "FreeBSD")
    set(CMAKE_REQUIRED_FLAGS "-L/usr/local/lib/gcc47")
  endif ()
  find_package(BLAS)
  unset(CMAKE_REQUIRED_FLAGS)
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
