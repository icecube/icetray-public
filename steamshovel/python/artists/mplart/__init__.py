# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

try:
    from .QtMPLArtist import QtMPLArtist as MPLArtist
except ImportError:
    from .AggMPLArtist import AggMPLArtist as MPLArtist
