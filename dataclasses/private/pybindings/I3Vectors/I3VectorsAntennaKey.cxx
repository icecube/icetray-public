// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include "I3Vectors.h"
#include <dataclasses/AntennaKey.h>

void register_I3VectorAntennaKey()
{
  register_i3vector_of<AntennaKey>("AntennaKey");
}
