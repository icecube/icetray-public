if (SYSTEM_PACKAGES)
  find_path(GMP_INCLUDE_DIR
    NAMES include/gmp.h)
  tooldef(gmp
    ${GMP_INCLUDE_DIR}
    gmp.h
    lib
    NONE
    gmp
  )
endif (SYSTEM_PACKAGES)

if (NOT GMP_FOUND)
  set(GMP_VERSION "4.3.2")
  tooldef(gmp
    include/gmp-${GMP_VERSION}/gmp
    gmp.h
    lib/gmp-${GMP_VERSION}
    NONE
    gmp
  )
endif (NOT GMP_FOUND)
