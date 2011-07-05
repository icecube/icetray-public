colormsg("")
colormsg(HICYAN "lapack")

set(BLA_STATIC FALSE)
enable_language(Fortran)
find_package(LAPACK)
