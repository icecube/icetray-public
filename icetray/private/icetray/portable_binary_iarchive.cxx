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

#include <istream>

#include <boost/archive/portable_binary_iarchive.hpp>

#include <boost/archive/impl/basic_binary_iarchive.ipp>
#include <boost/archive/basic_binary_iprimitive.hpp> // contains init(), etc
#include <boost/archive/impl/basic_binary_iprimitive.ipp> // contains init(), etc
#include <boost/archive/impl/archive_pointer_iserializer.ipp>

#if BOOST_VERSION < 103600
#define BOOST_ARCHIVE_SOURCE
#endif

namespace boost {
namespace archive {
  
#if BOOST_VERSION > 103301
  template class basic_binary_iprimitive<portable_binary_iarchive, std::istream::char_type, std::istream::traits_type> ;
#else
  template class basic_binary_iprimitive<portable_binary_iarchive, std::istream> ;
#endif
  template class basic_binary_iarchive<portable_binary_iarchive> ;
  //  template class portable_binary_iarchive_impl<portable_binary_iarchive> ;
  template class detail::archive_pointer_iserializer<portable_binary_iarchive> ;

} // namespace archive
} // namespace boost

