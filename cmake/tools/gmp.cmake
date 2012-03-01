set(GMP_VERSION "4.3.2")
tooldef(gmp
  include/gmp-${GMP_VERSION}
  gmp/gmp.h
  lib/gmp-${GMP_VERSION}
  NONE
  gmp
  )
