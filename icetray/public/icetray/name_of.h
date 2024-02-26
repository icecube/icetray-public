// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef ICETRAY_NAME_OF_H_INCLUDED
#define ICETRAY_NAME_OF_H_INCLUDED

#ifndef __CINT__
#include <string>
#include <typeinfo>

namespace icetray
{
  const std::string& name_of(const std::type_info &ti);

  template <typename T>
  const std::string& name_of()
  {
    return name_of(typeid(T));
  }
}

#endif
#endif
