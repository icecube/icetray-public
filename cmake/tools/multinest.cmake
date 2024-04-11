# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

#
# experimental multinest tool detection
# (only tested on OS X for homebrew multinest
# installed from https://raw.githubusercontent.com/ntessore/homebrew-nt/master/multinest.rb
# so far.)
#

tooldef(multinest
    include
    multinest.h
    lib
    NONE
    multinest
)
