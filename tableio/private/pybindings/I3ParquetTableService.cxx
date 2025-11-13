/**
 * I3ParquetTableService.cxx (pybindings)
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
#include <tableio/parquetwriter/I3ParquetTableService.h>

namespace bp = boost::python;


void register_I3ParquetTableService() {

   bp::class_<I3ParquetTableService,
            boost::shared_ptr<I3ParquetTableService>,
            bp::bases<I3TableService>>
            ("I3ParquetTableService",
            bp::init<std::string, std::string>(bp::args("folder_path", "compression")))
            ;
}
