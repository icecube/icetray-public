#ifndef BOOST_ARCHIVE_PORTABLE_BINARY_OARCHIVE_HPP
#define BOOST_ARCHIVE_PORTABLE_BINARY_OARCHIVE_HPP

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

#ifndef BOOST_ARCHIVE_CUSTOM_OARCHIVE_TYPES
#define BOOST_ARCHIVE_CUSTOM_OARCHIVE_TYPES boost::archive::portable_binary_oarchive
#endif

#include <boost/archive/archive_exception.hpp>
#include <boost/archive/basic_binary_oarchive.hpp>
#include <boost/archive/detail/portable_binary_archive.hpp>
#include <boost/archive/basic_binary_oprimitive.hpp>

#include <sys/types.h>
#include <iostream>
#include <vector>
#include <cstring>

namespace boost {
  namespace archive {

    template<class Archive>
    class portable_binary_oarchive_impl : 
#if BOOST_VERSION > 103301
      public basic_binary_oprimitive<Archive, std::ostream::char_type, std::ostream::traits_type>,
#else
      public basic_binary_oprimitive<Archive, std::ostream>,
#endif
      public basic_binary_oarchive<Archive>
    {
      friend class detail::interface_oarchive<Archive>;
      friend class basic_binary_oarchive<Archive>;
      friend class save_access;

    protected:

      // note: the following should not needed - but one compiler (vc 7.1)
      // fails to compile one test (test_shared_ptr) without it !!!
      // make this protected so it can be called from a derived archive
//      template<class T>
//      void save_override(T & t, BOOST_PFTO int){
//        basic_binary_oarchive<Archive>::save_override(t, 0);
//      }

      portable_binary_oarchive_impl(std::ostream & os) :
#if BOOST_VERSION > 103301
        basic_binary_oprimitive<Archive, std::ostream::char_type, std::ostream::traits_type>(*os.rdbuf(), true),
#else
        basic_binary_oprimitive<Archive, std::ostream>(os, true),
#endif
        basic_binary_oarchive<Archive>(no_codecvt)
      { }
    };



    class portable_binary_oarchive : public portable_binary_archive,
				     public portable_binary_oarchive_impl<portable_binary_oarchive>
    {
      typedef portable_binary_oarchive self_t;
      typedef boost::archive::portable_binary_oarchive_impl<self_t> base_t;

      friend class boost::archive::basic_binary_oarchive<self_t>;
#if BOOST_VERSION > 103301
      friend class boost::archive::basic_binary_oprimitive<self_t, std::ostream::char_type, std::ostream::traits_type>;
#else
      friend class boost::archive::basic_binary_oprimitive<self_t, std::ostream>;
#endif
      friend class boost::archive::detail::interface_oarchive<self_t>;
      friend class boost::archive::save_access;
    protected:

#ifndef BOOST_PORTABLE_BINARY_ARCHIVE_BIG_ENDIAN

      template <typename T>
      void save_override(const T& t, unsigned version)
      {
	base_t::save_override(t, version);
      }

      template <typename T>
      void 
      save_fast(const std::vector<T>& v) 
      {
	uint32_t l = v.size();
	save(l);
	save_binary(&(v.front()), l * sizeof(T));
      }

#define PORTABLE_BINARY_OARCHIVE_FAST_VECTOR_DISPATCH(T) \
      void save_override(const std::vector<T>& t, unsigned version) { save_fast(t); }

      PORTABLE_BINARY_OARCHIVE_FAST_VECTOR_DISPATCH(char);
      PORTABLE_BINARY_OARCHIVE_FAST_VECTOR_DISPATCH(unsigned char);
      PORTABLE_BINARY_OARCHIVE_FAST_VECTOR_DISPATCH(short);
      PORTABLE_BINARY_OARCHIVE_FAST_VECTOR_DISPATCH(unsigned short);
      PORTABLE_BINARY_OARCHIVE_FAST_VECTOR_DISPATCH(int);
      PORTABLE_BINARY_OARCHIVE_FAST_VECTOR_DISPATCH(unsigned int);
      // note conspicuouly absent 'long'... it changes size from platform to platform
      PORTABLE_BINARY_OARCHIVE_FAST_VECTOR_DISPATCH(long long);
      PORTABLE_BINARY_OARCHIVE_FAST_VECTOR_DISPATCH(unsigned long long);
      PORTABLE_BINARY_OARCHIVE_FAST_VECTOR_DISPATCH(float);
      PORTABLE_BINARY_OARCHIVE_FAST_VECTOR_DISPATCH(double);
      
#endif
	
      // default fall through for any types not specified here
      template<class T>
      void save(const T & t) {
        base_t::save(t);
      }

      // this should be for PPC only
      void save(bool b)
      {
	unsigned char c = b;
	base_t::save(c);
      }

      void save(short t)
      {
	swap(t);
        base_t::save(t);
      }

      void save(unsigned short t)
      {
	swap(t);
        base_t::save(t);
      }

      void save(int t)
      {
	swap(t);
        base_t::save(t);
      }

      void save(unsigned int t)
      {
	swap(t);
        base_t::save(t);
      }

      void save(long t)
      {
	// we know it's in range, we can upsize, swap, and save
	int64_t int64 = t;
	swap(int64);
        base_t::save(int64);
      }

      void save(unsigned long t)
      {
	// we know it's in range, we can upsize, swap, and save
	uint64_t uint64 = t;
	swap(uint64);
        base_t::save(uint64);
      }

      void save(long long t)
      {
	swap(t);
        base_t::save(t);
      }

      void save(unsigned long long t)
      {
	swap(t);
        base_t::save(t);
      }

      void save(float f)
      {
	swap(f);
	base_t::save(f);
      }

      void save(double d)
      {
	swap(d);
        base_t::save(d);
      }

      void save(long double ld)
      {
	throw portable_binary_archive_exception
	  (portable_binary_archive_exception::long_double_not_supported);
      }

      void save(const char * s)
      {
	uint32_t l = std::strlen(s);
	save(l); // gets swapped inside save
	save_binary(s, l);
      }

      void save(const std::string & s)
      {
	uint32_t l = static_cast<uint32_t>(s.size());
	save(l); // gets swapped inside save()
	save_binary(s.data(), l);
      }

    public:
      portable_binary_oarchive(std::ostream & os) :
	base_t(os)
      { }
    };
  }
}
#endif // BOOST_ARCHIVE_PORTABLE_BINARY_OARCHIVE_HPP
