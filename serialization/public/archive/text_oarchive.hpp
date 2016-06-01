#ifndef I3_ARCHIVE_TEXT_OARCHIVE_HPP
#define I3_ARCHIVE_TEXT_OARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// text_oarchive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <ostream>
#include <cstddef> // std::size_t

#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::size_t; 
} // namespace std
#endif

#include <archive/detail/auto_link_archive.hpp>
#include <archive/basic_text_oprimitive.hpp>
#include <archive/basic_text_oarchive.hpp>
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
class text_oarchive_impl : 
     /* protected ? */ public basic_text_oprimitive<std::ostream>,
     public basic_text_oarchive<Archive>
{
#ifdef BOOST_NO_MEMBER_TEMPLATE_FRIENDS
public:
#else
protected:
    #if BOOST_WORKAROUND(BOOST_MSVC, < 1500)
        // for some inexplicable reason insertion of "class" generates compile erro
        // on msvc 7.1
        friend detail::interface_oarchive<Archive>;
        friend basic_text_oarchive<Archive>;
        friend save_access;
    #else
        friend class detail::interface_oarchive<Archive>;
        friend class basic_text_oarchive<Archive>;
        friend class save_access;
    #endif
#endif
    template<class T>
    void save(const T & t){
        this->newtoken();
        basic_text_oprimitive<std::ostream>::save(t);
    }
    void save(const version_type & t){
        save(static_cast<const unsigned int>(t));
    }
    void save(const icecube::serialization::item_version_type & t){
        save(static_cast<const unsigned int>(t));
    }
    I3_ARCHIVE_DECL(void) 
    save(const char * t);
    #ifndef BOOST_NO_INTRINSIC_WCHAR_T
    I3_ARCHIVE_DECL(void) 
    save(const wchar_t * t);
    #endif
    I3_ARCHIVE_DECL(void) 
    save(const std::string &s);
    #ifndef BOOST_NO_STD_WSTRING
    I3_ARCHIVE_DECL(void) 
    save(const std::wstring &ws);
    #endif
    I3_ARCHIVE_DECL(BOOST_PP_EMPTY()) 
    text_oarchive_impl(std::ostream & os, unsigned int flags);
    // don't import inline definitions! leave this as a reminder.
    //I3_ARCHIVE_DECL(BOOST_PP_EMPTY()) 
    ~text_oarchive_impl(){};
public:
    I3_ARCHIVE_DECL(void) 
    save_binary(const void *address, std::size_t count);
};

// do not derive from this class.  If you want to extend this functionality
// via inhertance, derived from text_oarchive_impl instead.  This will
// preserve correct static polymorphism.
class text_oarchive : 
    public text_oarchive_impl<text_oarchive>
{
public:
    text_oarchive(std::ostream & os_, unsigned int flags = 0) :
        // note: added _ to suppress useless gcc warning
        text_oarchive_impl<text_oarchive>(os_, flags)
    {}
    ~text_oarchive(){}
};

} // namespace archive
} // namespace icecube

// required by export
I3_SERIALIZATION_REGISTER_ARCHIVE(icecube::archive::text_oarchive)

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

#include <archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#endif // I3_ARCHIVE_TEXT_OARCHIVE_HPP
