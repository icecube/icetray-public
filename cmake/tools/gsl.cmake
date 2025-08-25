# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

colormsg("")
colormsg(CYAN "gsl")
find_package(GSL)
if (GSL_FOUND)
  found_ok("  version: ${GSL_VERSION}")
  found_ok("  include: ${GSL_INCLUDE_DIRS}")
  found_ok("libraries: ${GSL_LIBRARIES}")
  else(GSL_FOUND)
    message(WARNING "GSL not found")
endif(GSL_FOUND)
