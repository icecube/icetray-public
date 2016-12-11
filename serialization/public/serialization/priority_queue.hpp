#ifndef I3_SERIALIZATION_PRIORITY_QUEUE_HPP
#define I3_SERIALIZATION_PRIORITY_QUEUE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// priority_queue.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <queue>
#include <boost/config.hpp>

// function specializations must be defined in the appropriate
// namespace - icecube::serialization
#if defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)
#define STD _STLP_STD
#else
#define STD std
#endif

namespace icecube { 
namespace serialization {
namespace detail{

template <typename U, typename Container, typename Compare>
struct priority_queue_save : public STD::priority_queue<U, Container, Compare> {
    template<class Archive>
    void operator()(Archive & ar, const unsigned int file_version) const {
        save(ar, STD::priority_queue<U, Container, Compare>::c, file_version);
    }
};
template <typename U, typename Container, typename Compare>
struct priority_queue_load : public STD::priority_queue<U, Container, Compare> {
    template<class Archive>
    void operator()(Archive & ar, const unsigned int file_version) {
        load(ar, STD::priority_queue<U, Container, Compare>::c, file_version);
    }
};

} // detail

template<class Archive, class T, class Container, class Compare>
inline void serialize(
    Archive & ar,
    std::priority_queue< T, Container, Compare> & t,
    const unsigned int file_version 
){
    typedef typename boost::mpl::eval_if<
        typename Archive::is_saving,
        boost::mpl::identity<detail::priority_queue_save<T, Container, Compare> >,
        boost::mpl::identity<detail::priority_queue_load<T, Container, Compare> >
    >::type typex;
    static_cast<typex &>(t)(ar, file_version);
}

} // namespace serialization
} // namespace icecube

#include <serialization/collection_traits.hpp>

I3_SERIALIZATION_COLLECTION_TRAITS(STD::priority_queue)

#undef STD

#endif // I3_SERIALIZATION_PRIORITY_QUEUE_HPP
