/**
 * I3SQLiteTableService.cxx (pybindings)
 *
 * Copyright  (C) 2025 The Icecube Collaboration
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Aurora Wohlfahrt <lepeler@icecube.wisc.edu> Last changed by: $LastChangedBy$
 */

#include "pybindings.hpp"
#include <tableio/sqlitewriter/I3SQLiteTableService.h>

namespace bp = boost::python;


void register_I3SQLiteTableService() {

   bp::class_<I3SQLiteTableService,
             boost::shared_ptr<I3SQLiteTableService>,
             bp::bases<I3TableService>>
             ("I3SQLiteTableService",
              bp::init<std::string>(bp::args("path")))
             ;
}
