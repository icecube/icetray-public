
TOOLDEF (suitesparse
  include
  cholmod.h
  lib
  NONE
  camd ccolamd spqr cholmod amd colamd tbb suitesparseconfig
)

if (NOT SUITESPARSE_FOUND)
  TOOLDEF (suitesparse
      include
      suitesparse/cholmod.h 
      lib
      NONE
      camd ccolamd spqr cholmod amd colamd suitesparseconfig
  )
endif (NOT SUITESPARSE_FOUND)

if (NOT SUITESPARSE_FOUND)
  # Try again without libsuitesparseconfig
  TOOLDEF (suitesparse
      include
      suitesparse/cholmod.h 
      lib
      NONE
      camd ccolamd spqr cholmod amd colamd
  )
endif (NOT SUITESPARSE_FOUND)

if (SUITESPARSE_FOUND)
  LIST(APPEND SUITESPARSE_INCLUDE_DIR ${SUITESPARSE_INCLUDE_DIR}/suitesparse)
endif (SUITESPARSE_FOUND)

