# SPDX-FileCopyrightText: 2025 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import warnings
from icecube.tableio import I3_USE_ROOT

if I3_USE_ROOT:
    from icecube.tableio import I3ROOTTableService, I3ROOTWriter

warnings.warn(
    "icecube.rootwriter is deprecated and will be removed in a future release. Use icecube.tableio instead.",
    DeprecationWarning,
    stacklevel=2,
)

__all__ = [
    "I3ROOTTableService",
    "I3ROOTWriter"
]
