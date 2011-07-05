colormsg("")
colormsg(HICYAN "blas")

enable_language(Fortran OPTIONAL) 
set(BLA_STATIC FALSE)
find_package(BLAS)
