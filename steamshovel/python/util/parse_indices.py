# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import re

def parse_indices( s ):
    indices = set()
    for a, b in re.findall( r"([0-9]+)\s*-\s*([0-9]+)", s ):
        indices.update( range( int(a), int(b) ) )
    for x in re.findall("[0-9]+", s):
        indices.add( int(x) )
    return list(indices)
