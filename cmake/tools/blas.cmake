find_library(BLAS_LIB NAMES goto2 goto acml mkl blas PATHS ${TOOL_SYSTEM_PATH})

if (NOT BLAS_LIB)
	set(BLAS_FOUND FALSE CACHE BOOL "Tool BLAS found successfully" FORCE)
else (NOT BLAS_LIB)
	set(BLAS_LIBRARIES "${BLAS_LIB}" CACHE PATH "Libraries for tool BLAS" FORCE)
	set(BLAS_FOUND TRUE CACHE BOOL "Tool BLAS found successfully" FORCE)
endif (NOT BLAS_LIB)
