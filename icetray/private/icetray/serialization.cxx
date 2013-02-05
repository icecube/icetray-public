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

#include <iostream>

#include <icetray/portable_binary_archive.hpp>
#if BOOST_VERSION < 104000
#include <boost/archive/impl/archive_pointer_iserializer.ipp>
#include <boost/archive/impl/archive_pointer_oserializer.ipp>

namespace boost {
namespace archive {
  
  template class detail::archive_pointer_iserializer<portable_binary_iarchive> ;
  template class detail::archive_pointer_oserializer<portable_binary_oarchive> ;

} // namespace archive
} // namespace boost
#else
#include <boost/archive/impl/archive_serializer_map.ipp>

namespace boost {
namespace archive {
  
  template class detail::archive_serializer_map<portable_binary_iarchive> ;
  template class detail::archive_serializer_map<portable_binary_oarchive> ;

} // namespace archive
} // namespace boost
#endif

