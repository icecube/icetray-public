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

#ifndef BOOST_ARCHIVE_CUSTOM_IARCHIVE_TYPES
#define BOOST_ARCHIVE_CUSTOM_IARCHIVE_TYPES boost::archive::portable_binary_iarchive
#endif

#include <boost/archive/archive_exception.hpp>
#include <boost/archive/basic_binary_iarchive.hpp>
#include <boost/archive/detail/portable_binary_archive.hpp>
#include <boost/archive/basic_binary_iprimitive.hpp>

#include <iostream>
#include <limits>
#include <vector>

namespace boost {
  namespace archive {

    template<class Archive>
    class portable_binary_iarchive_impl : 
#if BOOST_VERSION > 103301 
      public basic_binary_iprimitive<Archive, std::istream::char_type, std::istream::traits_type>,
#else
      public basic_binary_iprimitive<Archive, std::istream>,
#endif
      public basic_binary_iarchive<Archive>
    {
      friend class detail::interface_iarchive<Archive>;
      friend class basic_binary_iarchive<Archive>;
      friend class load_access;

    protected:

      // note: the following should not needed - but one compiler (vc 7.1)
      // fails to compile one test (test_shared_ptr) without it !!!
      // make this protected so it can be called from a derived archive
//      template<class T>
//      void load_override(T & t, BOOST_PFTO int){
//        basic_binary_iarchive<Archive>::load_override(t, 0);
//      }

      portable_binary_iarchive_impl(std::istream & is, unsigned int flags) :
#if BOOST_VERSION > 103301	
        basic_binary_iprimitive<Archive, std::istream::char_type, std::istream::traits_type>(*is.rdbuf(), true),
#else
        basic_binary_iprimitive<Archive, std::istream>(is, true),
#endif
        basic_binary_iarchive<Archive>(no_codecvt)
      {
      }
    };

    class portable_binary_iarchive : public portable_binary_archive,
				     public portable_binary_iarchive_impl<portable_binary_iarchive>
    {
      typedef portable_binary_iarchive self_t;
      typedef boost::archive::portable_binary_iarchive_impl<self_t> base_t;

      friend class boost::archive::basic_binary_iarchive<self_t>;
#if BOOST_VERSION > 103301
      friend class boost::archive::basic_binary_iprimitive<self_t, std::istream::char_type, std::istream::traits_type>;
#else
      friend class boost::archive::basic_binary_iprimitive<self_t, std::istream>;
#endif
      friend class boost::archive::load_access;
      friend class boost::archive::detail::interface_iarchive<self_t>;

    protected:

#ifndef BOOST_PORTABLE_BINARY_ARCHIVE_BIG_ENDIAN

      template <typename T>
      void load_override(T& t, unsigned version)
      {
	base_t::load_override(t, version);
      }

      template <typename T>
      void 
      load_fast(std::vector<T>& v)
      {
	uint32_t l;
	base_t::load(l);
	v.resize(l);
	// this data() call is standard but a little too new
	//	load_binary(v.data(), l * sizeof(T));
	load_binary(&(v.front()), l * sizeof(T));
      }

      void 
      load_fast(std::string& s)
      {
	uint32_t l;
	base_t::load(l);
	s.resize(l);
	load_binary(&(s[0]), l);
      }

      void load_override(std::string& s, unsigned version) { load_fast(s); }

#define PORTABLE_BINARY_IARCHIVE_FAST_VECTOR_DISPATCH(T) \
      void load_override(std::vector<T>& t, unsigned version) { load_fast(t); }

      PORTABLE_BINARY_IARCHIVE_FAST_VECTOR_DISPATCH(char);
      PORTABLE_BINARY_IARCHIVE_FAST_VECTOR_DISPATCH(unsigned char);
      PORTABLE_BINARY_IARCHIVE_FAST_VECTOR_DISPATCH(short);
      PORTABLE_BINARY_IARCHIVE_FAST_VECTOR_DISPATCH(unsigned short);
      PORTABLE_BINARY_IARCHIVE_FAST_VECTOR_DISPATCH(int);
      PORTABLE_BINARY_IARCHIVE_FAST_VECTOR_DISPATCH(unsigned int);
      // note conspicuously absent 'long'...  it changes size from
      // platform to platform
      PORTABLE_BINARY_IARCHIVE_FAST_VECTOR_DISPATCH(long long);
      PORTABLE_BINARY_IARCHIVE_FAST_VECTOR_DISPATCH(unsigned long long);
      PORTABLE_BINARY_IARCHIVE_FAST_VECTOR_DISPATCH(float);
      PORTABLE_BINARY_IARCHIVE_FAST_VECTOR_DISPATCH(double);

#endif

      // default fall through for any types not specified here
      template<class T>
      void load(T & t) 
      {
        base_t::load(t);
      }

      void load(bool & b) 
      {
	unsigned char c;
	base_t::load(c);
	b = c;
      }

      void load(short & t) 
      {
	base_t::load(t);
	swap(t);
      }

      void load (unsigned short & t)
      {
	base_t::load(t);
	swap(t);
      }

      void load(int & t) 
      {
	base_t::load(t);
	swap(t);
      }

      void load (unsigned int & t)
      {
	base_t::load(t);
	swap(t);
      }

      // longs are always written as 32 bits
      void load(long & t)
      {
	int64_t int64;
	base_t::load(int64);
	swap(int64);
#if LONG_MAX < INT64_MAX
	if (int64 < LONG_MIN || int64 > LONG_MAX)
	  throw portable_binary_archive_exception
	    (portable_binary_archive_exception::long_int_out_of_range);
#endif
	t = int64;

      }

      void load(unsigned long & t)
      {
	uint64_t uint64;
	base_t::load(uint64);
	swap(uint64);
#if ULONG_MAX < UINT64_MAX
	if (uint64 > ULONG_MAX)
	  throw portable_binary_archive_exception
	    (portable_binary_archive_exception::long_int_out_of_range);
#endif
	t = uint64;
      }

      void load(long long & t)
      {
	base_t::load(t);
	swap(t);
      }

      void load(unsigned long long & t)
      {
	base_t::load(t);
	swap(t);
      }

      void load (float & f)
      {
	base_t::load(f);
	swap(f);
      }

      void load (double & d)
      {
	base_t::load(d);
	swap(d);
      }

      void load (char * s)
      {
	uint32_t l;
	load(l);
	load_binary(s, l);
	s[l] = '\0';
      }
      
      void load (std::string & s)
      {
	uint32_t l;
	load(l);
	s.resize(l);
	// breaking a rule for efficiency's sake.
	load_binary(const_cast<char *>(s.data()), l);
      }

    public:
      portable_binary_iarchive(std::istream & is) :
        base_t(is, 0)
      { }
    };

  }
}

#endif // BOOST_ARCHIVE_PORTABLE_BINARY_IARCHIVE_HPP
