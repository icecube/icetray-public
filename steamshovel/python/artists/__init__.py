# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import os

# Automatically list most modules in this directory

ignore = ("__init__.py", "util.py")

__all__ = [os.path.basename(x[:-3]) for x in
             os.listdir(os.path.dirname(__file__))
           if x.endswith(".py") and x not in ignore]
