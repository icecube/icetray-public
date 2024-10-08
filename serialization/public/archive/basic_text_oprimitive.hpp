#ifndef I3_ARCHIVE_BASIC_TEXT_OPRIMITIVE_HPP
#define I3_ARCHIVE_BASIC_TEXT_OPRIMITIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_text_oprimitive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// SPDX-License-Identifier: BSL-1.0
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// archives stored as text - note these ar templated on the basic
// stream templates to accommodate wide (and other?) kind of characters
//
// note the fact that on libraries without wide characters, ostream is
// is not a specialization of basic_ostream which in fact is not defined
// in such cases.   So we can't use basic_ostream<OStream::char_type> but rather
// use two template parameters

#include <iomanip>
#include <locale>
#include <boost/assert.hpp>
#include <cstddef> // size_t

#include <boost/config.hpp>
#include <boost/static_assert.hpp>
#include <boost/detail/workaround.hpp>
#include <boost/io/ios_state.hpp>

#if BOOST_WORKAROUND(BOOST_DINKUMWARE_STDLIB, == 1)
#include <archive/dinkumware.hpp>
#endif

#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::size_t;
    #if ! defined(BOOST_DINKUMWARE_STDLIB) && ! defined(__SGI_STL_PORT)
        using ::locale;
    #endif
} // namespace std
#endif

#include <boost/type_traits/is_floating_point.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/limits.hpp>
#include <boost/integer.hpp>
#include <boost/io/ios_state.hpp>
#include <boost/scoped_ptr.hpp>
#include <serialization/throw_exception.hpp>
#include <archive/archive_exception.hpp>
#include <archive/basic_streambuf_locale_saver.hpp>
#include <archive/detail/abi_prefix.hpp> // must be the last header

namespace icecube {
namespace archive {

class save_access;

/////////////////////////////////////////////////////////////////////////
// class basic_text_oprimitive - output of prmitives to stream
template<class OStream>
class basic_text_oprimitive
{
protected:
    OStream &os;
    boost::io::ios_flags_saver flags_saver;
    boost::io::ios_precision_saver precision_saver;

    #ifndef BOOST_NO_STD_LOCALE
    boost::scoped_ptr<std::locale> archive_locale;
    basic_streambuf_locale_saver<
        typename OStream::char_type,
        typename OStream::traits_type
    > locale_saver;
    #endif

    /////////////////////////////////////////////////////////
    // fundamental types that need special treatment
    void save(const bool t){
        // trap usage of invalid uninitialized boolean which would
        // otherwise crash on load.
        BOOST_ASSERT(0 == static_cast<int>(t) || 1 == static_cast<int>(t));
        if(os.fail())
            icecube::serialization::throw_exception(
                archive_exception(archive_exception::output_stream_error)
            );
        os << t;
    }
    void save(const signed char t)
    {
        save(static_cast<short int>(t));
    }
    void save(const unsigned char t)
    {
        save(static_cast<short unsigned int>(t));
    }
    void save(const char t)
    {
        save(static_cast<short int>(t));
    }
    #ifndef BOOST_NO_INTRINSIC_WCHAR_T
    void save(const wchar_t t)
    {
        BOOST_STATIC_ASSERT(sizeof(wchar_t) <= sizeof(int));
        save(static_cast<int>(t));
    }
    #endif

    /////////////////////////////////////////////////////////
    // saving of any types not listed above

    template<class T>
    void save_impl(const T &t, boost::mpl::bool_<false> &){
        if(os.fail())
            icecube::serialization::throw_exception(
                archive_exception(archive_exception::output_stream_error)
            );
        os << t;
    }

    /////////////////////////////////////////////////////////
    // floating point types need even more special treatment
    // the following determines whether the type T is some sort
    // of floating point type.  Note that we then assume that
    // the stream << operator is defined on that type - if not
    // we'll get a compile time error. This is meant to automatically
    // support synthesized types which support floating point
    // operations. Also it should handle compiler dependent types
    // such long double.  Due to John Maddock.

    template<class T>
    struct is_float {
        typedef typename boost::mpl::bool_<
            boost::is_floating_point<T>::value
            || (std::numeric_limits<T>::is_specialized
            && !std::numeric_limits<T>::is_integer
            && !std::numeric_limits<T>::is_exact
            && std::numeric_limits<T>::max_exponent)
        >::type type;
    };

    template<class T>
    void save_impl(const T &t, boost::mpl::bool_<true> &){
        // must be a user mistake - can't serialize un-initialized data
        if(os.fail()) {
            icecube::serialization::throw_exception(
                archive_exception(archive_exception::output_stream_error)
            );
	}
        // The formulae for the number of decimla digits required is given in
        // http://www2.open-std.org/JTC1/SC22/WG21/docs/papers/2005/n1822.pdf
        // which is derived from Kahan's paper:
        // www.eecs.berkeley.edu/~wkahan/ieee754status/ieee754.ps
        // const unsigned int digits = (std::numeric_limits<T>::digits * 3010) / 10000;
        // note: I've commented out the above because I didn't get good results.  e.g.
        // in one case I got a difference of 19 units.
//        #ifndef BOOST_NO_CXX11_NUMERIC_LIMITS
//            const unsigned int digits = std::numeric_limits<T>::max_digits10;
//        #else
            const unsigned int digits = std::numeric_limits<T>::digits10 + 2;
//        #endif
        os << std::setprecision(digits) << std::scientific << t;
    }

    template<class T>
    void save(const T & t){
        boost::io::ios_flags_saver fs(os);
        boost::io::ios_precision_saver ps(os);
        typename is_float<T>::type tf;
        save_impl(t, tf);
    }

    I3_ARCHIVE_OR_WARCHIVE_DECL(BOOST_PP_EMPTY())
    basic_text_oprimitive(OStream & os, bool no_codecvt);
    I3_ARCHIVE_OR_WARCHIVE_DECL(BOOST_PP_EMPTY())
    ~basic_text_oprimitive();
public:
    // unformatted append of one character
    void put(typename OStream::char_type c){
        if(os.fail())
            icecube::serialization::throw_exception(
                archive_exception(archive_exception::output_stream_error)
            );
        os.put(c);
    }
    // unformatted append of null terminated string
    void put(const char * s){
        while('\0' != *s)
            os.put(*s++);
    }
    I3_ARCHIVE_OR_WARCHIVE_DECL(void)
    save_binary(const void *address, std::size_t count);
};

} //namespace icecube
} //namespace archive

#include <archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#endif // I3_ARCHIVE_BASIC_TEXT_OPRIMITIVE_HPP
