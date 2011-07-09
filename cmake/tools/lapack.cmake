## If you're having trouble with detection on a debian/ubuntu
## multiarch system (Ubuntu 11.04+), uncomment the lines around the
## find_package() call.

colormsg("")
colormsg(HICYAN "lapack")

set(BLA_STATIC FALSE)
#set(CMAKE_REQUIRED_FLAGS "-L/usr/lib/i386-linux-gnu -L/usr/lib/x86_64-linux-gnu -lgfortran")
find_package(LAPACK)
#unset(CMAKE_REQUIRED_FLAGS)

## uncomment the following for debugging...
#colormsg("BLAS_LIBRARIES: ${BLAS_LIBRARIES}")
#colormsg("LAPACK_LIBRARIES: ${LAPACK_LIBRARIES}")
