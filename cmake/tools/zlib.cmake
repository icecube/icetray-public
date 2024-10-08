# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

colormsg("")
colormsg(HICYAN "zlib")
find_package(ZLIB QUIET REQUIRED)

if(ZLIB_FOUND)
  found_ok("zlib.h found at ${ZLIB_INCLUDE_DIRS}")
  found_ok(${ZLIB_LIBRARIES})
endif()
