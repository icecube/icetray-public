#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import os
import sys

retval = os.system('icetray-inspect icetray dataclasses phys-services')

if retval != 0:
    sys.exit(1)



