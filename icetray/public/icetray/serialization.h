/**
 *  $Id$
 *  
 *  Copyright (C) 2007
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
#ifndef ICETRAY_SERIALIZATION_H_INCLUDED
#define ICETRAY_SERIALIZATION_H_INCLUDED

#include <boost/version.hpp>

#ifndef __CINT__

#include <icetray/i3_extended_type_info.h>
#include <archive/xml_iarchive.hpp>
#include <archive/xml_oarchive.hpp>
#include <archive/portable_binary_archive.hpp>

#include <serialization/serialization.hpp>
#include <serialization/nvp.hpp>
#include <serialization/base_object.hpp>
// #include <serialization/is_abstract.hpp>
#include <serialization/access.hpp>
#include <serialization/export.hpp>
#include <serialization/vector.hpp>
#include <serialization/string.hpp>
#include <serialization/map.hpp>
#include <serialization/shared_ptr.hpp>
#include <serialization/utility.hpp>
#include <serialization/split_member.hpp>
#include <serialization/version.hpp>

#include <icetray/is_shared_ptr.h>
#include <boost/utility/enable_if.hpp>

#include <sstream>

using icecube::serialization::make_nvp;
using icecube::serialization::base_object;

template <typename T>
std::string
AsXML(const T& t)
{
  std::ostringstream oss;
  icecube::archive::xml_oarchive xoa(oss, icecube::archive::no_header);
  xoa << make_nvp("obj", t);
  return oss.str();
}

//

#define I3_BASIC_SERIALIZABLE(T) \
  template void T::serialize(icecube::archive::portable_binary_oarchive&, unsigned); \
  template void T::serialize(icecube::archive::portable_binary_iarchive&, unsigned); \
  template void T::serialize(icecube::archive::xml_iarchive&, unsigned);	\
  template void T::serialize(icecube::archive::xml_oarchive&, unsigned);

#define I3_EXPORT(T)				\
  static i3_export_key_setter<T> BOOST_PP_CAT(i3_export_key_setter_, __LINE__) (BOOST_PP_STRINGIZE(T));	\
  I3_CLASS_EXPORT(T);			\
  I3_SERIALIZATION_SHARED_PTR(T);

#define I3_SERIALIZABLE(T)						\
  I3_BASIC_SERIALIZABLE(T)						\
  template std::string AsXML(const T&);					\
  template std::string AsXML(boost::shared_ptr<T> const&);		\
  template std::string AsXML(boost::shared_ptr<const T> const&);	\
  I3_EXPORT(T)

#define I3_SPLIT_SERIALIZABLE(T)					\
  I3_SERIALIZABLE(T)							    \
  template void T::save(icecube::archive::portable_binary_oarchive&, unsigned) const; \
  template void T::load(icecube::archive::portable_binary_iarchive&, unsigned); \
  template void T::load(icecube::archive::xml_iarchive&, unsigned);	\
  template void T::save(icecube::archive::xml_oarchive&, unsigned) const;
  

#else // __CINT__

#define I3_CLASS_VERSION(T,V) 
#define I3_IS_ABSTRACT(X)
#define I3_CLASS_EXPORT(X) 
#define I3_SHARED_POINTER_EXPORT(X) 
#define I3_SERIALIZATION_SPLIT_MEMBER()

namespace icecube
{
  namespace serialization 
  {
    // normal forward declarations:
    template <class T> struct nvp;
    template <class T> const nvp<T> make_nvp(const char* name, T& t);

    template <class Retval, class Derived>
      Retval base_object(Derived);
  }
}
using icecube::serialization::make_nvp;

#endif

#endif //BOOSTHEADERS_H_INCLUDED
