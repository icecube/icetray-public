// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef ICETRAY_PYTHON_ICETRAY_STREAM_OVERLOADS_HPP_INCLUDED
#define ICETRAY_PYTHON_ICETRAY_STREAM_OVERLOADS_HPP_INCLUDED

#include <iostream>
template <typename T, typename U>
std::ostream& operator<<(std::ostream& oss, std::pair<T, U> p){
  oss << "("<<p.first<<", "<<p.second<<")";
  return oss;
}

#endif // ICETRAY_PYTHON_ICETRAY_STREAM_OVERLOADS_HPP_INCLUDED
