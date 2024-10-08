// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include "pybindings.hpp"
#include <dataclasses/physics/I3LinearizedMCTree.h>

namespace bp = boost::python;

void register_I3LinearizedMCTree()
{
	bp::class_<I3LinearizedMCTree, I3LinearizedMCTreePtr, bp::bases<I3MCTree> >("I3LinearizedMCTree")
	    .def(bp::init<const I3MCTree &>())
	;

	register_pointer_conversions<I3LinearizedMCTree>();
}
