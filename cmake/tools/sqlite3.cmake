# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

tooldef(sqlite3
    include
    sqlite3.h
    lib
    NONE # The bin directory is n/a, placeholder
    sqlite3
)

if(SQLITE3_FOUND)
  add_definitions(-DI3_USE_SQLITE3)
endif(SQLITE3_FOUND)
