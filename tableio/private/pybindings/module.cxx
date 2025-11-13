/**
 * tableio pybindings
 *
 * Copyright  (C) 2010 The Icecube Collaboration
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy$
 */

#include "pybindings.hpp"
#include <icetray/load_project.h>

namespace bp = boost::python;

// only register classes depending on root, if it is found
#ifdef I3_USE_ROOT
  #define REGISTER_ROOT_DEPENDENT (I3ROOTTableService)
#else
  #define REGISTER_ROOT_DEPENDENT
#endif

// only register classes depending on hdf5, if it is found
#ifdef I3_USE_HDF5
  #define REGISTER_HDF5_DEPENDENT (I3HDFTableService)
#else
  #define REGISTER_HDF5_DEPENDENT
#endif

// only register classes depending on sqlite3, if it is found
#ifdef I3_USE_SQLITE3
  #define REGISTER_SQLITE3_DEPENDENT (I3SQLiteTableService)
#else
  #define REGISTER_SQLITE3_DEPENDENT
#endif

// only register classes depending on arrow, if it is found
#ifdef I3_USE_ARROW
  #define REGISTER_ARROW_DEPENDENT (I3ParquetTableService)
#else
  #define REGISTER_ARROW_DEPENDENT
#endif

#define REGISTER_THESE_THINGS \
   (I3TableRowDescription)(I3TableRow)(I3Converter)(I3TableService)     \
   (I3TableWriter)(I3TableTranscriber)(I3ConverterBundle)(I3Datatype)   \
   (I3Table)(I3BroadcastTableService)(I3CSVTableService)                \
    REGISTER_ROOT_DEPENDENT                                             \
    REGISTER_HDF5_DEPENDENT                                             \
    REGISTER_SQLITE3_DEPENDENT                                          \
    REGISTER_ARROW_DEPENDENT

#define I3_REGISTRATION_FN_DECL(r, data, t) void BOOST_PP_CAT(register_,t)();
#define I3_REGISTER(r, data, t) BOOST_PP_CAT(register_,t)();
BOOST_PP_SEQ_FOR_EACH(I3_REGISTRATION_FN_DECL, ~, REGISTER_THESE_THINGS)

I3_PYTHON_MODULE(tableio)
{
  load_project("tableio", false);
  bp::import("icecube._dataio");

  BOOST_PP_SEQ_FOR_EACH(I3_REGISTER, ~, REGISTER_THESE_THINGS);

  // forward the I3_USE_ROOT definition
  #ifdef I3_USE_ROOT
    bp::scope().attr("I3_USE_ROOT")=true;
  #else
    bp::scope().attr("I3_USE_ROOT")=false;
  #endif

  // forward the I3_USE_HDF5 definition
  #ifdef I3_USE_HDF5
    bp::scope().attr("I3_USE_HDF5")=true;
  #else
    bp::scope().attr("I3_USE_HDF5")=false;
  #endif

  // forward the I3_USE_SQLITE3 definition
  #ifdef I3_USE_SQLITE3
    bp::scope().attr("I3_USE_SQLITE3")=true;
  #else
    bp::scope().attr("I3_USE_SQLITE3")=false;
  #endif

  // forward the I3_USE_ARROW definition
  #ifdef I3_USE_ARROW
    bp::scope().attr("I3_USE_ARROW")=true;
  #else
    bp::scope().attr("I3_USE_ARROW")=false;
  #endif

}
