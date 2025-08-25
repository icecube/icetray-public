# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

## This is the tool detection file for BLAS and LAPACK

colormsg("")
colormsg(CYAN "blas and lapack")

find_package(BLAS)

if(BLAS_FOUND)
  found_ok("     BLAS libraries: ${BLAS_LIBRARIES}")
  found_ok("  BLAS linker flags: ${BLAS_LINKER_FLAGS}")
  find_package(LAPACK)
  if (LAPACK_FOUND)
    found_ok("   LAPACK libraries: ${LAPACK_LIBRARIES}")
    found_ok("LAPACK linker flags: ${LAPACK_LINKER_FLAGS}")
  else(LAPACK_FOUND)
    message(WARNING "LAPACK not found")
  endif(LAPACK_FOUND)
else(BLAS_FOUND)
    message(WARNING "BLAS not found")
endif(BLAS_FOUND)
