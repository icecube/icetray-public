// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include <dataclasses/I3Vector.h>

// specialize I3Vector<char> because ...well it's special
std::ostream& operator<<(std::ostream& oss, const I3Vector<char> l){
  oss << std::string(l.begin(),l.end());
 return oss;
}

