#ifndef I3_ARCHIVE_BINARY_OARCHIVE_IMPL_HPP
#define I3_ARCHIVE_BINARY_OARCHIVE_IMPL_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// binary_oarchive_impl.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// SPDX-License-Identifier: BSL-1.0
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <ostream>
#include <boost/config.hpp>
#include <serialization/pfto.hpp>
#include <archive/basic_binary_oprimitive.hpp>
#include <archive/basic_binary_oarchive.hpp>

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

namespace icecube {
namespace archive {

namespace detail {
    template<class Archive> class interface_oarchive;
} // namespace detail

template<class Archive, class Elem, class Tr>
class binary_oarchive_impl :
    public basic_binary_oprimitive<Archive, Elem, Tr>,
    public basic_binary_oarchive<Archive>
{
#ifdef BOOST_NO_MEMBER_TEMPLATE_FRIENDS
public:
#else
protected:
    #if BOOST_WORKAROUND(BOOST_MSVC, < 1500)
        // for some inexplicable reason insertion of "class" generates compile erro
        // on msvc 7.1
        friend detail::interface_oarchive<Archive>;
        friend basic_binary_oarchive<Archive>;
        friend save_access;
    #else
        friend class detail::interface_oarchive<Archive>;
        friend class basic_binary_oarchive<Archive>;
        friend class save_access;
    #endif
#endif
    // note: the following should not needed - but one compiler (vc 7.1)
    // fails to compile one test (test_shared_ptr) without it !!!
    // make this protected so it can be called from a derived archive
    template<class T>
    void save_override(T & t, I3_PFTO int){
        this->basic_binary_oarchive<Archive>::save_override(t, 0L);
    }
    void init(unsigned int flags) {
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
    binary_oarchive_impl(
        std::basic_streambuf<Elem, Tr> & bsb,
        unsigned int flags
    ) :
        basic_binary_oprimitive<Archive, Elem, Tr>(
            bsb,
            0 != (flags & no_codecvt)
        ),
        basic_binary_oarchive<Archive>(flags)
    {
        init(flags);
    }
    binary_oarchive_impl(
        std::basic_ostream<Elem, Tr> & os,
        unsigned int flags
    ) :
        basic_binary_oprimitive<Archive, Elem, Tr>(
            * os.rdbuf(),
            0 != (flags & no_codecvt)
        ),
        basic_binary_oarchive<Archive>(flags)
    {
        init(flags);
    }
};

} // namespace archive
} // namespace icecube

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

#endif // I3_ARCHIVE_BINARY_OARCHIVE_IMPL_HPP
