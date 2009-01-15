#ifndef BOOST_ARCHIVE_PORTABLE_BINARY_IARCHIVE_HPP
#define BOOST_ARCHIVE_PORTABLE_BINARY_IARCHIVE_HPP

//
//  copyright (c) 2005, 2006, 2007
//  troy d. straszheim <troy@resophonic.com>
//  http:/www.resophonic.com
//
// 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/config for most recent version.
//
//  $Id$
//

#include <istream>
#include <boost/archive/portable_binary_iarchive_impl.hpp>
#include <boost/archive/shared_ptr_helper.hpp>
#include <boost/archive/detail/register_archive.hpp>

namespace boost {
  namespace archive {

    class portable_binary_iarchive : 
      public portable_binary_iarchive_impl<
      boost::archive::portable_binary_iarchive, 
      std::istream::char_type, 
      std::istream::traits_type
      >,
      public detail::shared_ptr_helper
    {

    public:
      
      portable_binary_iarchive(std::istream & is, unsigned int flags = 0) :
        portable_binary_iarchive_impl<
      portable_binary_iarchive, std::istream::char_type, std::istream::traits_type
      >(is, flags)
      {}

      portable_binary_iarchive(std::streambuf & bsb, unsigned int flags = boost::archive::no_header) :
        portable_binary_iarchive_impl<
      portable_binary_iarchive, std::istream::char_type, std::istream::traits_type
      >(bsb, flags)
      {}

    };
	
  } // namespace archive
} // namespace boost

BOOST_SERIALIZATION_REGISTER_ARCHIVE(boost::archive::portable_binary_iarchive);
BOOST_SERIALIZATION_USE_ARRAY_OPTIMIZATION(boost::archive::portable_binary_iarchive);



#endif // BOOST_ARCHIVE_PORTABLE_BINARY_IARCHIVE_HPP


