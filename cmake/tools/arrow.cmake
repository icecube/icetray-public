# SPDX-FileCopyrightText: 2025 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

tooldef(arrow
    include
    arrow/api.h
    lib
    NONE # The bin directory is n/a, placeholder
    arrow
    parquet
)

if(ARROW_FOUND)
  add_definitions(-DI3_USE_ARROW)
endif(ARROW_FOUND)
