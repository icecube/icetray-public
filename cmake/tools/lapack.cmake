colormsg("")
colormsg(HICYAN "lapack")

enable_language(Fortran OPTIONAL) 
set(BLA_STATIC FALSE)
find_package(LAPACK)
