#ifndef BOOST_PORTABLE_BINARY_ARCHIVE_HPP
#define BOOST_PORTABLE_BINARY_ARCHIVE_HPP

//
//  copyright (c) 2005
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

#include <sys/types.h>
#include <boost/assert.hpp>

#if (BYTE_ORDER == BIG_ENDIAN)
#define BOOST_PORTABLE_BINARY_ARCHIVE_BIG_ENDIAN 1
#endif

namespace boost {
  namespace archive {

    class portable_binary_archive_exception : 
      public virtual boost::archive::archive_exception
    {
    public:
      typedef enum {
        long_int_out_of_range,
	long_double_not_supported,
	checksum_failure
      } exception_code;
      exception_code code;
      portable_binary_archive_exception(exception_code c)
	: code(c) { }
      virtual const char *what( ) const throw( )
      {
        const static char* programmer_error_msg = "programmer error";
        const static char* long_int_out_of_range_msg = "long int out of range";
        const static char* long_double_not_supported_msg = "long double not supported";
        const static char* checksum_failure_msg = "corruption: checksum failure";
	
	const char* msg;
        switch(code){
        case long_int_out_of_range:
	  msg = long_int_out_of_range_msg;
	  break;
        case long_double_not_supported:
	  msg = long_double_not_supported_msg;
	  break;
        case checksum_failure:
	  msg = checksum_failure_msg;
	  break;
        default:
	  msg = programmer_error_msg;
        }
        return msg;
      }
    };

    namespace portable {
      
      template <int size>
      struct swap_impl 
      { 
      };

      template <>
      struct swap_impl<1>
      {
	template <typename T>
	  static void swap(T&) 
	{ }
      };

      template<>
      struct swap_impl<2>
      {
	template <typename T>
	  static void swap(T& swapit)
	{
	  BOOST_STATIC_ASSERT(sizeof(T) == 2);
	  swapit = 
	    ((swapit & 0x00FF) << 8) 
	    | ((swapit & 0xFF00) >> 8);
	}
      };

      template<>
      struct swap_impl<4>
      {
	template <typename T>
	  static void swap(T& arg)
	{
	  BOOST_STATIC_ASSERT(sizeof(T) == 4);
	  uint32_t& swapit(reinterpret_cast<uint32_t&>(arg));
	  swapit = ((swapit & 0x000000FF) << 24) 
	    | ((swapit & 0x0000FF00) << 8 ) 
	    | ((swapit & 0x00FF0000) >> 8 ) 
	    | ((swapit & 0xFF000000) >> 24);
	}
      };

      template<>
      struct swap_impl<8>
      {
	template <typename T>
	  static void swap(T& arg)
	{
	  BOOST_STATIC_ASSERT(sizeof(T) == 8);
	  uint64_t& swapit(reinterpret_cast<uint64_t&>(arg));
	  swapit = 
	    ((swapit & 0x00000000000000FFll) << 56) |
	    ((swapit & 0x000000000000FF00ll) << 40) |
	    ((swapit & 0x0000000000FF0000ll) << 24) |
	    ((swapit & 0x00000000FF000000ll) << 8 ) |
	    ((swapit & 0x000000FF00000000ll) >> 8 ) |
	    ((swapit & 0x0000FF0000000000ll) >> 24) |
	    ((swapit & 0x00FF000000000000ll) >> 40) |
	    ((swapit & 0xFF00000000000000ll) >> 56);
	}
      };
    }

    //
    // common fns/typedefs used by portable_binary_(i|o)archive
    //
    class portable_binary_archive
    {

    protected:

      template <typename T>
      inline void swap(T& t)
      {
#ifdef BOOST_PORTABLE_BINARY_ARCHIVE_BIG_ENDIAN
	portable::swap_impl<sizeof(T)>::swap(t);
#endif
      }

    public:

      portable_binary_archive() { }
    };
  }
}

#endif // BOOST_PORTABLE_BINARY_ARCHIVE_HPP
