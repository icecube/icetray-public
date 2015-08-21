#ifndef ICETRAY_I3DEPRECATION_H_INCLUDED
#define ICETRAY_I3DEPRECATION_H_INCLUDED

/**
 *  $Id$
 *
 *  Copyright (C) 2010
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 */

#include <icetray/I3Factory.h>

void emitDeprecationWarning(const char* kind, const char* name);

template <class FactoryProductType, class ModuleType>
struct DeprecatedCreate
{
  static
  boost::shared_ptr<FactoryProductType>
  Create (const I3Context& c)
  {
    std::string productstr = I3::name_of<FactoryProductType>();
    std::string modulestr = I3::name_of<ModuleType>();
    emitDeprecationWarning(productstr.c_str(), modulestr.c_str());
    boost::shared_ptr<FactoryProductType> module(new ModuleType(c));
    if (!module)
      log_fatal("failed to create");

    return module;
  }
};

namespace {
  struct I3DeprecationWarningCaller {
    I3DeprecationWarningCaller() {
      emitDeprecationWarning("project", BOOST_PP_STRINGIZE(PROJECT)); // PROJECT set on commandline
    }
  };
}

#define I3_DEPRECATED_PROJECT()                                         \
  namespace {                                                           \
    static I3DeprecationWarningCaller const&                            \
    I3DeprecationWarningInstance =                                      \
      I3::Singleton<I3DeprecationWarningCaller>::get_mutable_instance(); \
  }

#define I3_DEPRECATED_MODULE(TYPE) I3_REGISTER(I3Module, TYPE, DeprecatedCreate)
#define I3_DEPRECATED_SERVICE_FACTORY(TYPE) I3_REGISTER(I3ServiceFactory, TYPE, DeprecatedCreate)

#endif
