#ifndef BOOST_ARCHIVE_PORTABLE_BINARY_IARCHIVE_IMPL_HPP
#define BOOST_ARCHIVE_PORTABLE_BINARY_IARCHIVE_IMPL_HPP

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
#include <boost/pfto.hpp>
#include <boost/archive/basic_binary_iprimitive.hpp>
#include <boost/archive/basic_binary_iarchive.hpp>

#include <boost/serialization/collection_size_type.hpp>

namespace boost {
  namespace archive {

    template<class Archive, class Elem, class Tr>
    class portable_binary_iarchive_impl : 
      public basic_binary_iprimitive<Archive, Elem, Tr>,
      public basic_binary_iarchive<Archive>
    {
      friend class detail::interface_iarchive<Archive>;
      friend class basic_binary_iarchive<Archive>;
      friend class load_access;

    protected:

      portable_binary_iarchive_impl(std::basic_streambuf<Elem, Tr> & bsb, 
				    unsigned int flags) 
	: basic_binary_iprimitive<Archive, Elem, Tr>(bsb, 0 != (flags & no_codecvt)),
	  basic_binary_iarchive<Archive>(flags)
      { }

      portable_binary_iarchive_impl(std::basic_istream<Elem, Tr> & is, 
				    unsigned int flags) 
	: basic_binary_iprimitive<Archive, Elem, Tr>(* is.rdbuf(), 0 != (flags & no_codecvt)),
	  basic_binary_iarchive<Archive>(flags)
      { }
      
      template<class T>
      void 
      load_override(T & t, BOOST_PFTO int)
      {
        this->basic_binary_iarchive<Archive>::load_override(t, 0L);
      }

      void load_override(std::string& s, BOOST_PFTO int)
      {
	uint32_t l;
	this->load(l);
	s.resize(l);
	this->load_binary(&(s[0]), l);
      }

      void load_override(class_name_type& t, BOOST_PFTO int)
      {
	std::string cn;
	cn.reserve(BOOST_SERIALIZATION_MAX_KEY_SIZE);
	this->load_override(cn, 0);
	if(cn.size() > (BOOST_SERIALIZATION_MAX_KEY_SIZE - 1))
	  boost::throw_exception(archive_exception(archive_exception::invalid_class_name));
	std::memcpy(t, cn.data(), cn.size());
	// borland tweak
	t.t[cn.size()] = '\0';
      }

      void load_override(boost::serialization::collection_size_type& t, BOOST_PFTO int)
      {
	uint32_t l;
	this->load(l);
	t = l;
      }
    };
  }
}

#endif // BOOST_ARCHIVE_PORTABLE_PORTABLE_BINARY_IARCHIVE_HPP
