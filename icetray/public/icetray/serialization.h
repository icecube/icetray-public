/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *  
 *  SPDX-License-Identifier: BSD-2-Clause
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
  xoa << icecube::serialization::make_nvp("obj", t);
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
