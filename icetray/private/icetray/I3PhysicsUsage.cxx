// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include <icetray/I3PhysicsUsage.h>
#include <ostream>

std::ostream&
operator<<(std::ostream& os, const I3PhysicsUsage& ru)
{
  os << "[Physics Usage systime:" << ru.systime
     << " usertime:" << ru.usertime
     << " ncall:" << ru.ncall << "]";
  return os;
}
