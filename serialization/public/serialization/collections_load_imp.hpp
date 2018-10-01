#ifndef I3_SERIALIZATION_COLLECTIONS_LOAD_IMP_HPP
#define I3_SERIALIZATION_COLLECTIONS_LOAD_IMP_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#if defined(_MSC_VER) && (_MSC_VER <= 1020)
#  pragma warning (disable : 4786) // too long name, harmless warning
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// collections_load_imp.hpp: serialization for loading stl collections

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// helper function templates for serialization of collections

#include <cassert>
#include <cstddef> // size_t
#include <boost/version.hpp>
#include <boost/config.hpp> // msvc 6.0 needs this for warning suppression
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::size_t;
} // namespace std
#endif
#include <boost/detail/workaround.hpp>

#include <serialization/access.hpp>
#include <serialization/nvp.hpp>
#include <serialization/detail/stack_constructor.hpp>
#include <serialization/collection_size_type.hpp>
#include <serialization/item_version_type.hpp>

namespace icecube{
namespace serialization {
namespace stl {

//////////////////////////////////////////////////////////////////////
// implementation of serialization for STL containers
//

// sequential container input
template<class Archive, class Container>
struct archive_input_seq
{
    inline void operator()(
        Archive &ar,
        Container &s,
        const unsigned int v
    ){
        typedef BOOST_DEDUCED_TYPENAME Container::value_type type;
        detail::stack_construct<Archive, type> t(ar, v);
        // borland fails silently w/o full namespace
        ar >> icecube::serialization::make_nvp("item", t.reference());
        s.push_back(t.reference());
        ar.reset_object_address(& s.back() , & t.reference());
    }
};

// map input
template<class Archive, class Container>
struct archive_input_map
{
    inline void operator()(
        Archive &ar,
        Container &s,
        const unsigned int v
    ){
        typedef BOOST_DEDUCED_TYPENAME Container::value_type type;
        detail::stack_construct<Archive, type> t(ar, v);
        // borland fails silently w/o full namespace
        ar >> icecube::serialization::make_nvp("item", t.reference());
        std::pair<BOOST_DEDUCED_TYPENAME Container::const_iterator, bool> result =
            s.insert(t.reference());
        // note: the following presumes that the map::value_type was NOT tracked
        // in the archive.  This is the usual case, but here there is no way
        // to determine that.
        if(result.second){
            ar.reset_object_address(
                & (result.first->second),
                & t.reference().second
            );
        }
    }
};

// multimap input
template<class Archive, class Container>
struct archive_input_multimap
{
    inline void operator()(
        Archive &ar,
        Container &s,
        const unsigned int v
    ){
        typedef BOOST_DEDUCED_TYPENAME Container::value_type type;
        detail::stack_construct<Archive, type> t(ar, v);
        // borland fails silently w/o full namespace
        ar >> icecube::serialization::make_nvp("item", t.reference());
        BOOST_DEDUCED_TYPENAME Container::const_iterator result
            = s.insert(t.reference());
        // note: the following presumes that the map::value_type was NOT tracked
        // in the archive.  This is the usual case, but here there is no way
        // to determine that.
        ar.reset_object_address(
            & result->second,
            & t.reference()
        );
    }
};

// set input
template<class Archive, class Container>
struct archive_input_set
{
    inline void operator()(
        Archive &ar,
        Container &s,
        const unsigned int v
    ){
        typedef BOOST_DEDUCED_TYPENAME Container::value_type type;
        detail::stack_construct<Archive, type> t(ar, v);
        // borland fails silently w/o full namespace
        ar >> icecube::serialization::make_nvp("item", t.reference());
        std::pair<BOOST_DEDUCED_TYPENAME Container::const_iterator, bool> result =
            s.insert(t.reference());
        if(result.second)
            ar.reset_object_address(& (* result.first), & t.reference());
    }
};

// multiset input
template<class Archive, class Container>
struct archive_input_multiset
{
    inline void operator()(
        Archive &ar,
        Container &s,
        const unsigned int v
    ){
        typedef BOOST_DEDUCED_TYPENAME Container::value_type type;
        detail::stack_construct<Archive, type> t(ar, v);
        // borland fails silently w/o full namespace
        ar >> icecube::serialization::make_nvp("item", t.reference());
        BOOST_DEDUCED_TYPENAME Container::const_iterator result
            = s.insert(t.reference());
        ar.reset_object_address(& (* result), & t.reference());
    }
};

template<class Container>
class reserve_imp
{
public:
    void operator()(Container &s, std::size_t count) const {
        s.reserve(count);
    }
};

template<class Container>
class no_reserve_imp
{
public:
    void operator()(Container & /* s */, std::size_t /* count */) const{}
};

template<class Archive, class Container, class InputFunction, class R>
inline void load_collection(Archive & ar, Container &s)
{
    s.clear();
    // retrieve number of elements
    collection_size_type count;
    unsigned int item_version;
    ar >> I3_SERIALIZATION_NVP(count);
    /*
      I3_HACK
    if(3 < ar.get_library_version())
        ar >> I3_SERIALIZATION_NVP(item_version);
    else
    */
    item_version = 0;
    R rx;
    rx(s, count);
    std::size_t c = count;
    InputFunction ifunc;
    while(c-- > 0){
        ifunc(ar, s, item_version);
    }
}

/*
#if BOOST_VERSION >= 105900
template<class Archive, class Container, class InputFunction, class R>
inline void collection_load_impl(
    Archive & ar,
    Container & t,
    collection_size_type count,
    item_version_type item_version
){
    load_collection<Archive,Container,InputFunction,R>(ar,t);
}
#endif
*/

/*
#if BOOST_VERSION >= 105900
template<
    class Archive,
    class T
>
typename boost::enable_if<
    typename detail::is_default_constructible<
        typename T::value_type
    >,
    void
>::type
collection_load_impl(
    Archive & ar,
    T & t,
    collection_size_type count,
    item_version_type item_version
){
    t.resize(count);
    typename T::iterator hint;
    hint = t.begin();
    while(count-- > 0){
        ar >> icecube::serialization::make_nvp("item", *hint++);
    }
}

template<
    class Archive,
    class T
>
typename boost::disable_if<
    typename detail::is_default_constructible<
        typename T::value_type
    >,
    void
>::type
collection_load_impl(
    Archive & ar,
    T & t,
    collection_size_type count,
    item_version_type item_version
){
    t.clear();
    while(count-- > 0){
        detail::stack_construct<Archive, typename T::value_type> u(ar, item_version);
        ar >> icecube::serialization::make_nvp("item", u.reference());
        t.push_back(u.reference());
        ar.reset_object_address(& t.back() , & u.reference());
     }
}
#endif
*/

} // namespace stl
} // namespace serialization
} // namespace icecube

#endif //I3_SERIALIZATION_COLLECTIONS_LOAD_IMP_HPP
