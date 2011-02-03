find_library(LAPACK_LIB NAMES goto2 lapack)

if (NOT LAPACK_LIB)
	set(LAPACK_FOUND FALSE CACHE BOOL "Tool LAPACK found successfully" FORCE)
else (NOT LAPACK_LIB)
	set(LAPACK_LIBRARIES "${LAPACK_LIB}" CACHE PATH "Libraries for tool LAPACK" FORCE)
	set(LAPACK_FOUND TRUE CACHE BOOL "Tool LAPACK found successfully" FORCE)
endif (NOT LAPACK_LIB)
