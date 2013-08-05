set(GMP_VERSION "4.3.2")
tooldef(gmp
  include/gmp-${GMP_VERSION}
  gmp/gmp.h
  lib/gmp-${GMP_VERSION}
  NONE
  gmp
  )
if(NOT GMP_FOUND)
  set(GMP_LIBRARIES)
  tooldef(gmp
    "include/x86_64-linux-gnu"
    gmp.h
    "lib/x86_64-liunx-gnu"
    NONE
    gmp
    )
endif(NOT GMP_FOUND)