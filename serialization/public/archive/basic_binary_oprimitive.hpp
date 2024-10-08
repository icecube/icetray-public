#ifndef I3_ARCHIVE_BASIC_BINARY_OPRIMITIVE_HPP
#define I3_ARCHIVE_BASIC_BINARY_OPRIMITIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_binary_oprimitive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// SPDX-License-Identifier: BSL-1.0
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// archives stored as native binary - this should be the fastest way
// to archive the state of a group of obects.  It makes no attempt to
// convert to any canonical form.

// IN GENERAL, ARCHIVES CREATED WITH THIS CLASS WILL NOT BE READABLE
// ON PLATFORM APART FROM THE ONE THEY ARE CREATE ON

#include <iosfwd>
#include <boost/assert.hpp>
#include <locale>
#include <streambuf> // basic_streambuf
#include <string>
#include <cstddef> // size_t

#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::size_t;
} // namespace std
#endif

#include <boost/cstdint.hpp>
#include <boost/integer.hpp>
#include <boost/integer_traits.hpp>
#include <boost/scoped_ptr.hpp>
#include <serialization/throw_exception.hpp>

#include <archive/basic_streambuf_locale_saver.hpp>
#include <archive/archive_exception.hpp>
#include <serialization/is_bitwise_serializable.hpp>
#include <boost/mpl/placeholders.hpp>
#include <serialization/array.hpp>
#include <archive/detail/auto_link_archive.hpp>
#include <archive/detail/abi_prefix.hpp> // must be the last header

namespace icecube {
namespace archive {

/////////////////////////////////////////////////////////////////////////
// class basic_binary_oprimitive - binary output of prmitives

template<class Archive, class Elem, class Tr>
class basic_binary_oprimitive {
#ifndef BOOST_NO_MEMBER_TEMPLATE_FRIENDS
    friend class save_access;
protected:
#else
public:
#endif
    std::basic_streambuf<Elem, Tr> & m_sb;
    // return a pointer to the most derived class
    Archive * This(){
        return static_cast<Archive *>(this);
    }
    #ifndef BOOST_NO_STD_LOCALE
    boost::scoped_ptr<std::locale> archive_locale;
    basic_streambuf_locale_saver<Elem, Tr> locale_saver;
    #endif
    // default saving of primitives.
    template<class T>
    void save(const T & t)
    {
        save_binary(& t, sizeof(T));
    }

    /////////////////////////////////////////////////////////
    // fundamental types that need special treatment

    // trap usage of invalid uninitialized boolean which would
    // otherwise crash on load.
    void save(const bool t){
        BOOST_ASSERT(0 == static_cast<int>(t) || 1 == static_cast<int>(t));
        save_binary(& t, sizeof(t));
    }
    I3_ARCHIVE_OR_WARCHIVE_DECL(void)
    save(const std::string &s);
    #ifndef BOOST_NO_STD_WSTRING
    I3_ARCHIVE_OR_WARCHIVE_DECL(void)
    save(const std::wstring &ws);
    #endif
    I3_ARCHIVE_OR_WARCHIVE_DECL(void)
    save(const char * t);
    I3_ARCHIVE_OR_WARCHIVE_DECL(void)
    save(const wchar_t * t);

    I3_ARCHIVE_OR_WARCHIVE_DECL(void)
    init();

    I3_ARCHIVE_OR_WARCHIVE_DECL(BOOST_PP_EMPTY())
    basic_binary_oprimitive(
        std::basic_streambuf<Elem, Tr> & sb,
        bool no_codecvt
    );
    I3_ARCHIVE_OR_WARCHIVE_DECL(BOOST_PP_EMPTY())
    ~basic_binary_oprimitive();
public:

    // we provide an optimized save for all fundamental types
    // typedef serialization::is_bitwise_serializable<mpl::_1>
    // use_array_optimization;
    // workaround without using mpl lambdas
    struct use_array_optimization {
        template <class T>
        #if defined(BOOST_NO_DEPENDENT_NESTED_DERIVATIONS)
            struct apply {
                typedef typename icecube::serialization::is_bitwise_serializable< T >::type type;
            };
        #else
            struct apply : public icecube::serialization::is_bitwise_serializable< T > {};
        #endif
    };


    // the optimized save_array dispatches to save_binary
    template <class ValueType>
    void save_array(icecube::serialization::array<ValueType> const& a, unsigned int)
    {
      save_binary(a.address(),a.count()*sizeof(ValueType));
    }

    void save_binary(const void *address, std::size_t count);
};

template<class Archive, class Elem, class Tr>
inline void
basic_binary_oprimitive<Archive, Elem, Tr>::save_binary(
    const void *address,
    std::size_t count
){
    //BOOST_ASSERT(
    //    static_cast<std::size_t>((std::numeric_limits<std::streamsize>::max)()) >= count
    //);
    // note: if the following assertions fail
    // a likely cause is that the output stream is set to "text"
    // mode where by cr characters recieve special treatment.
    // be sure that the output stream is opened with ios::binary
    //if(os.fail())
    //    icecube::serialization::throw_exception(
    //        archive_exception(archive_exception::output_stream_error)
    //    );
    // figure number of elements to output - round up
    count = ( count + sizeof(Elem) - 1)
        / sizeof(Elem);
    BOOST_ASSERT(count <= std::size_t(boost::integer_traits<std::streamsize>::const_max));
    std::streamsize scount = m_sb.sputn(
        static_cast<const Elem *>(address),
        static_cast<std::streamsize>(count)
    );
    if(count != static_cast<std::size_t>(scount))
        icecube::serialization::throw_exception(
            archive_exception(archive_exception::output_stream_error)
        );
    //os.write(
    //    static_cast<const typename OStream::char_type *>(address),
    //    count
    //);
    //BOOST_ASSERT(os.good());
}

} //namespace icecube
} //namespace archive

#include <archive/detail/abi_suffix.hpp> // pop pragmas

#endif // I3_ARCHIVE_BASIC_BINARY_OPRIMITIVE_HPP
