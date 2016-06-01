#ifndef I3_SERIALIZATION_COLLECTION_TRAITS_HPP
#define I3_SERIALIZATION_COLLECTION_TRAITS_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// collection_traits.hpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// This header assigns a level implemenation trait to a collection type
// for all primitives.  It is needed so that archives which are meant to be
// portable don't write class information in the archive.  Since, not all
// compiles recognize the same set of primitive types, the possibility
// exists for archives to be non-portable if class information for primitive
// types is included.  This is addressed by the following macros.
#include <boost/config.hpp>
#include <boost/mpl/integral_c.hpp>
#include <boost/mpl/integral_c_tag.hpp>

#include <boost/cstdint.hpp>
#include <boost/integer_traits.hpp>
#include <climits> // ULONG_MAX
#include <serialization/level.hpp>

#define I3_SERIALIZATION_COLLECTION_TRAITS_HELPER(T, C)          \
template<>                                                          \
struct implementation_level< C < T > > {                            \
    typedef boost::mpl::integral_c_tag tag;                                \
    typedef boost::mpl::int_<object_serializable> type;                    \
    BOOST_STATIC_CONSTANT(int, value = object_serializable);        \
};                                                                  \
/**/

#if defined(BOOST_NO_CWCHAR) || defined(BOOST_NO_INTRINSIC_WCHAR_T)
    #define I3_SERIALIZATION_COLLECTION_TRAITS_HELPER_WCHAR(C)
#else
    #define I3_SERIALIZATION_COLLECTION_TRAITS_HELPER_WCHAR(C)   \
    I3_SERIALIZATION_COLLECTION_TRAITS_HELPER(wchar_t, C)        \
    /**/
#endif

#if defined(BOOST_HAS_LONG_LONG)
    #define I3_SERIALIZATION_COLLECTION_TRAITS_HELPER_INT64(C)    \
    I3_SERIALIZATION_COLLECTION_TRAITS_HELPER(boost::long_long_type, C)  \
    I3_SERIALIZATION_COLLECTION_TRAITS_HELPER(boost::ulong_long_type, C) \
    /**/
#else
    #define I3_SERIALIZATION_COLLECTION_TRAITS_HELPER_INT64(C)
#endif

#define I3_SERIALIZATION_COLLECTION_TRAITS(C)                     \
    namespace icecube { namespace serialization {                      \
    I3_SERIALIZATION_COLLECTION_TRAITS_HELPER(bool, C)            \
    I3_SERIALIZATION_COLLECTION_TRAITS_HELPER(char, C)            \
    I3_SERIALIZATION_COLLECTION_TRAITS_HELPER(signed char, C)     \
    I3_SERIALIZATION_COLLECTION_TRAITS_HELPER(unsigned char, C)   \
    I3_SERIALIZATION_COLLECTION_TRAITS_HELPER(signed int, C)      \
    I3_SERIALIZATION_COLLECTION_TRAITS_HELPER(unsigned int, C)    \
    I3_SERIALIZATION_COLLECTION_TRAITS_HELPER(signed long, C)     \
    I3_SERIALIZATION_COLLECTION_TRAITS_HELPER(unsigned long, C)   \
    I3_SERIALIZATION_COLLECTION_TRAITS_HELPER(float, C)           \
    I3_SERIALIZATION_COLLECTION_TRAITS_HELPER(double, C)          \
    I3_SERIALIZATION_COLLECTION_TRAITS_HELPER(unsigned short, C)  \
    I3_SERIALIZATION_COLLECTION_TRAITS_HELPER(signed short, C)    \
    I3_SERIALIZATION_COLLECTION_TRAITS_HELPER_INT64(C)            \
    I3_SERIALIZATION_COLLECTION_TRAITS_HELPER_WCHAR(C)            \
    } }                                                              \
    /**/

#endif // I3_SERIALIZATION_COLLECTION_TRAITS_HPP
