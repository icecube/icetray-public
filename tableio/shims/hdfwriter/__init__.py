# SPDX-FileCopyrightText: 2025 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import warnings
from icecube.tableio import I3_USE_HDF5

if I3_USE_HDF5:
    from icecube.tableio import I3HDFTableService, I3HDFWriter, I3SimHDFWriter

warnings.warn(
    "icecube.hdfwriter is deprecated and will be removed in a future release. Use icecube.tableio instead.",
    DeprecationWarning,
    stacklevel=2,
)

__all__ = [
    "I3HDFTableService",
    "I3HDFWriter",
    "I3SimHDFWriter",
]
