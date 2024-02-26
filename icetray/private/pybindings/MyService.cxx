// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include <boost/python.hpp>

struct MyService{};
void register_MyService(){
  boost::python::class_<MyService>("MyService");
  boost::python::register_ptr_to_python< boost::shared_ptr<MyService> >();
}

