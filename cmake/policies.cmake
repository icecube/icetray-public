# SPDX-FileCopyrightText: 2026 The IceTray Contributors
# SPDX-License-Identifier: BSD-2-Clause

# set cmake policies. policy definitions can be found at
# https://cmake.org/cmake/help/latest/manual/cmake-policies.7.html
# or `man cmake-policies`

# policies set to "NEW" behavior
foreach(p CMP0002 CMP0042 CMP0046 CMP0053 CMP0054 CMP0074 CMP0094 CMP0167)
    if(POLICY ${p})
        cmake_policy(SET ${p} NEW)
    endif()
endforeach()
