#ifndef I3_ARCHIVE_XML_WOARCHIVE_HPP
#define I3_ARCHIVE_XML_WOARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// xml_woarchive.hpp

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

#include <cstddef> // size_t
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::size_t;
} // namespace std
#endif

#include <ostream>

#include <archive/detail/auto_link_warchive.hpp>
#include <archive/basic_text_oprimitive.hpp>
#include <archive/basic_xml_oarchive.hpp>
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
    template<class Archive> class interface_oarchive;
} // namespace detail

template<class Archive>
class xml_woarchive_impl :
    public basic_text_oprimitive<std::wostream>,
    public basic_xml_oarchive<Archive>
{
#ifdef BOOST_NO_MEMBER_TEMPLATE_FRIENDS
public:
#else
protected:
    #if BOOST_WORKAROUND(BOOST_MSVC, < 1500)
        // for some inexplicable reason insertion of "class" generates compile erro
        // on msvc 7.1
        friend detail::interface_oarchive<Archive>;
        friend basic_xml_oarchive<Archive>;
        friend save_access;
    #else
        friend class detail::interface_oarchive<Archive>;
        friend class basic_xml_oarchive<Archive>;
        friend class save_access;
    #endif
#endif

    //void end_preamble(){
    //    basic_xml_oarchive<Archive>::end_preamble();
    //}
    template<class T>
    void
    save(const T & t){
        basic_text_oprimitive<std::wostream>::save(t);
    }
    void
    save(const version_type & t){
        save(static_cast<const unsigned int>(t));
    }
    void
    save(const icecube::serialization::item_version_type & t){
        save(static_cast<const unsigned int>(t));
    }
    I3_WARCHIVE_DECL(void)
    save(const char * t);
    #ifndef BOOST_NO_INTRINSIC_WCHAR_T
    I3_WARCHIVE_DECL(void)
    save(const wchar_t * t);
    #endif
    I3_WARCHIVE_DECL(void)
    save(const std::string &s);
    #ifndef BOOST_NO_STD_WSTRING
    I3_WARCHIVE_DECL(void)
    save(const std::wstring &ws);
    #endif
    I3_WARCHIVE_DECL(BOOST_PP_EMPTY())
    xml_woarchive_impl(std::wostream & os, unsigned int flags);
    ~xml_woarchive_impl(){}
public:
    void
    save_binary(const void *address, std::size_t count){
        this->end_preamble();
        #if ! defined(__MWERKS__)
        this->basic_text_oprimitive<std::wostream>::save_binary(
        #else
        this->basic_text_oprimitive::save_binary(
        #endif
            address,
            count
        );
        this->indent_next = true;
    }
};

// we use the following because we can't use
// typedef xml_woarchive_impl<xml_woarchive_impl<...> > xml_woarchive;

// do not derive from this class.  If you want to extend this functionality
// via inhertance, derived from xml_woarchive_impl instead.  This will
// preserve correct static polymorphism.
class xml_woarchive :
    public xml_woarchive_impl<xml_woarchive>
{
public:
    xml_woarchive(std::wostream & os, unsigned int flags = 0) :
        xml_woarchive_impl<xml_woarchive>(os, flags)
    {}
    ~xml_woarchive(){}
};

} // namespace archive
} // namespace icecube

// required by export
I3_SERIALIZATION_REGISTER_ARCHIVE(icecube::archive::xml_woarchive)

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

#include <archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#endif // BOOST_NO_STD_WSTREAMBUF
#endif // I3_ARCHIVE_XML_WOARCHIVE_HPP
