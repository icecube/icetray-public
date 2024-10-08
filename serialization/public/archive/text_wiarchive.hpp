#ifndef I3_ARCHIVE_TEXT_WIARCHIVE_HPP
#define I3_ARCHIVE_TEXT_WIARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// text_wiarchive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// SPDX-License-Identifier: BSL-1.0
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <boost/config.hpp>
#ifdef BOOST_NO_STD_WSTREAMBUF
#error "wide char i/o not supported on this platform"
#else

#include <istream>

#include <archive/detail/auto_link_warchive.hpp>
#include <archive/basic_text_iprimitive.hpp>
#include <archive/basic_text_iarchive.hpp>
#include <archive/detail/register_archive.hpp>
#include <serialization/item_version_type.hpp>

#include <archive/detail/abi_prefix.hpp> // must be the last header

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

namespace icecube {
namespace archive {

namespace detail {
    template<class Archive> class interface_iarchive;
} // namespace detail

template<class Archive>
class text_wiarchive_impl :
    public basic_text_iprimitive<std::wistream>,
    public basic_text_iarchive<Archive>
{
#ifdef BOOST_NO_MEMBER_TEMPLATE_FRIENDS
public:
#else
protected:
    #if BOOST_WORKAROUND(BOOST_MSVC, < 1500)
        // for some inexplicable reason insertion of "class" generates compile erro
        // on msvc 7.1
        friend detail::interface_iarchive<Archive>;
        friend load_access;
    #else
        friend class detail::interface_iarchive<Archive>;
        friend class load_access;
    #endif
#endif
    template<class T>
    void load(T & t){
        basic_text_iprimitive<std::wistream>::load(t);
    }
    void load(version_type & t){
        unsigned int v;
        load(v);
        t = version_type(v);
    }
    void load(icecube::serialization::item_version_type & t){
        unsigned int v;
        load(v);
        t = icecube::serialization::item_version_type(v);
    }
    I3_WARCHIVE_DECL(void)
    load(char * t);
    #ifndef BOOST_NO_INTRINSIC_WCHAR_T
    I3_WARCHIVE_DECL(void)
    load(wchar_t * t);
    #endif
    I3_WARCHIVE_DECL(void)
    load(std::string &s);
    #ifndef BOOST_NO_STD_WSTRING
    I3_WARCHIVE_DECL(void)
    load(std::wstring &ws);
    #endif
    // note: the following should not needed - but one compiler (vc 7.1)
    // fails to compile one test (test_shared_ptr) without it !!!
    template<class T>
    void load_override(T & t, I3_PFTO int){
        basic_text_iarchive<Archive>::load_override(t, 0);
    }
    I3_WARCHIVE_DECL(BOOST_PP_EMPTY())
    text_wiarchive_impl(std::wistream & is, unsigned int flags);
    ~text_wiarchive_impl(){};
};

} // namespace archive
} // namespace icecube

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

#include <archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

namespace icecube {
namespace archive {

class text_wiarchive :
    public text_wiarchive_impl<text_wiarchive>{
public:
    text_wiarchive(std::wistream & is, unsigned int flags = 0) :
        text_wiarchive_impl<text_wiarchive>(is, flags)
    {}
    ~text_wiarchive(){}
};

} // namespace archive
} // namespace icecube

// required by export
I3_SERIALIZATION_REGISTER_ARCHIVE(icecube::archive::text_wiarchive)

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

#endif // BOOST_NO_STD_WSTREAMBUF
#endif // I3_ARCHIVE_TEXT_WIARCHIVE_HPP
