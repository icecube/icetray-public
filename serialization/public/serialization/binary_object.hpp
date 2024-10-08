#ifndef I3_SERIALIZATION_BINARY_OBJECT_HPP
#define I3_SERIALIZATION_BINARY_OBJECT_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// nvp.hpp: interface for serialization system.

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// SPDX-License-Identifier: BSL-1.0
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <boost/assert.hpp>

#include <cstddef> // std::size_t
#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::size_t;
} // namespace std
#endif

#include <boost/preprocessor/stringize.hpp>
#include <serialization/tracking.hpp>
#include <serialization/level.hpp>
#include <serialization/split_member.hpp>
#include <serialization/nvp.hpp>
#include <serialization/wrapper.hpp>

namespace icecube {
namespace serialization {

struct binary_object :
    public wrapper_traits<nvp<const binary_object> >
{
    void const * m_t;
    std::size_t m_size;
    template<class Archive>
    void save(Archive & ar, const unsigned int /* file_version */) const {
        ar.save_binary(m_t, m_size);
    }
    template<class Archive>
    void load(Archive & ar, const unsigned int /* file_version */) const {
        ar.load_binary(const_cast<void *>(m_t), m_size);
    }
    I3_SERIALIZATION_SPLIT_MEMBER()
    binary_object & operator=(const binary_object & rhs) {
        m_t = rhs.m_t;
        m_size = rhs.m_size;
        return *this;
    }
    binary_object(/* const */ void * const t, std::size_t size) :
        m_t(t),
        m_size(size)
    {}
    binary_object(const binary_object & rhs) :
        m_t(rhs.m_t),
        m_size(rhs.m_size)
    {}
};

// just a little helper to support the convention that all serialization
// wrappers follow the naming convention make_xxxxx
inline
#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
const
#endif
binary_object
make_binary_object(/* const */ void * t, std::size_t size){
    return binary_object(t, size);
}

} // namespace serialization
} // boost

#endif // I3_SERIALIZATION_BINARY_OBJECT_HPP
