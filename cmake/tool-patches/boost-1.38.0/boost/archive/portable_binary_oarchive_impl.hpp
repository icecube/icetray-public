#ifndef BOOST_ARCHIVE_PORTABLE_BINARY_OARCHIVE_IMPL_HPP
#define BOOST_ARCHIVE_PORTABLE_BINARY_OARCHIVE_IMPL_HPP

//
//  copyright (c) 2005, 2006, 2007
//  troy d. straszheim <troy@resophonic.com>
//  http:/www.resophonic.com
// 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/config for most recent version.
//
//  $Id$
//

#include <ostream>
#include <boost/pfto.hpp>
#include <boost/archive/basic_binary_oprimitive.hpp>
#include <boost/archive/basic_binary_oarchive.hpp>

#include <boost/serialization/collection_size_type.hpp>

#include <boost/archive/detail/portable_binary_archive.hpp>

namespace boost {
  namespace archive {

    template<class Archive, class Elem, class Tr>
    class portable_binary_oarchive_impl : 
      public basic_binary_oprimitive<Archive, Elem, Tr>,
      public basic_binary_oarchive<Archive>,
      public portable_binary_archive
    {
      friend class detail::interface_oarchive<Archive>;
      friend class basic_binary_oarchive<Archive>;
      friend class save_access;

    protected:

      // note: the following should not needed - but one compiler (vc 7.1)
      // fails to compile one test (test_shared_ptr) without it !!!
      // make this protected so it can be called from a derived archive
      void init(unsigned int flags) 
      {
        if(0 != (flags & no_header))
	  return;
#if ! defined(__MWERKS__)
	this->basic_binary_oarchive<Archive>::init();
	this->basic_binary_oprimitive<Archive, Elem, Tr>::init();
#else
	basic_binary_oarchive<Archive>::init();
	basic_binary_oprimitive<Archive, Elem, Tr>::init();
#endif
      }

      portable_binary_oarchive_impl(std::basic_streambuf<Elem, Tr> & bsb, 
				    unsigned int flags) 
	: basic_binary_oprimitive<Archive, Elem, Tr>(bsb, 0 != (flags & no_codecvt)),
	  basic_binary_oarchive<Archive>(flags)
      { }
      //        init(flags);
      //      }

      portable_binary_oarchive_impl(std::basic_ostream<Elem, Tr> & os, 
				    unsigned int flags) 
	: basic_binary_oprimitive<Archive, Elem, Tr>(* os.rdbuf(), 0 != (flags & no_codecvt)),
	  basic_binary_oarchive<Archive>(flags)
      { }
      //        init(flags);
      //      }

      template<class T>
      void 
      save_override(T & t, BOOST_PFTO int)
      {
        this->basic_binary_oarchive<Archive>::save_override(t, 0L);
      }

/* Swap byte order on big-endian systems */
#ifdef BOOST_PORTABLE_BINARY_ARCHIVE_BIG_ENDIAN
#define SWAP_STUB(type)								\
	void									\
	save_override(const type &t, BOOST_PFTO int)					\
	{									\
		type swapee(t);							\
		portable_binary_archive::swap(swapee);				\
		this->basic_binary_oarchive<Archive>::save_override(swapee, 0L);\
	}
#else
#define SWAP_STUB(type)
#endif

	SWAP_STUB(short)
	SWAP_STUB(unsigned short)
	SWAP_STUB(int)
	SWAP_STUB(unsigned int)
	SWAP_STUB(long)
	SWAP_STUB(unsigned long)
	SWAP_STUB(long long)
	SWAP_STUB(unsigned long long)
	SWAP_STUB(float)
	SWAP_STUB(double)

#undef SWAP_STUB

      void save_override(const bool& b, BOOST_PFTO int)
      {
	uint8_t l = b;
	this->save(l);
      }

      void save_override(const std::string & s, BOOST_PFTO int)
      {
	uint32_t l = static_cast<uint32_t>(s.size());
	this->save(l); // gets swapped inside save()
	this->save_binary(s.data(), l);
      }

      void save_override(const boost::serialization::collection_size_type& t, BOOST_PFTO int)
      {
	uint32_t l = t;
	this->save(l);
      }

      /*
      void save_override(const class_name_type & s, BOOST_PFTO int)
      {
	uint32_t l = static_cast<uint32_t>(s.size());
	this->save(l); // gets swapped inside save()
	this->save_binary(s.data(), l);
      }
      */
    };

  }
}

#endif // BOOST_ARCHIVE_PORTABLE_BINARY_OARCHIVE_HPP

