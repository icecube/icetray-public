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

#include <ostream>

#include <boost/archive/portable_binary_oarchive.hpp>

#include <boost/archive/basic_binary_oprimitive.hpp>
#include <boost/archive/impl/basic_binary_oarchive.ipp>
#include <boost/archive/impl/basic_binary_oprimitive.ipp> // contains init(), etc
#include <boost/archive/impl/archive_pointer_oserializer.ipp>

#if BOOST_VERSION < 103600
#define BOOST_ARCHIVE_SOURCE
#endif

namespace boost {
namespace archive {

#if BOOST_VERSION > 103301
  template class basic_binary_oprimitive<portable_binary_oarchive, std::ostream::char_type, std::ostream::traits_type> ;
#else
  template class basic_binary_oprimitive<portable_binary_oarchive, std::ostream> ;
#endif
  template class basic_binary_oarchive<portable_binary_oarchive> ;
  //  template class portable_binary_oarchive_impl<portable_binary_oarchive> ;
  template class detail::archive_pointer_oserializer<portable_binary_oarchive> ;

} // namespace archive
} // namespace boost


