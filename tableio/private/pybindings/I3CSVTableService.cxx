/**
 * Copyright  (C) 2010 The Icecube Collaboration
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * $Id$
 *
 * @version $$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$
 */

#include "pybindings.hpp"
#include "tableio/textwriter/I3CSVTableService.h"

namespace bp = boost::python;

void register_I3CSVTableService() {
    bp::class_<I3CSVTableService,
               boost::shared_ptr<I3CSVTableService>,
               bp::bases<I3TableService> >
               ("I3CSVTableService",
                bp::init<const std::string>(bp::args("folder_name")))
               ;
}
