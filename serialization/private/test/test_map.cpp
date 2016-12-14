/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_map.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// (C) Copyright 2014 Jim Bell
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

#include <algorithm> // std::copy
#include <vector>
#include <fstream>
#include <cstddef> // size_t, NULL

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>

#include <cstdio>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::rand; 
    using ::size_t;
}
#endif

#include <I3Test.h>

#include <serialization/nvp.hpp>
#include <serialization/map.hpp>

#include "A.hpp"
#include "A.ipp"

///////////////////////////////////////////////////////
// a key value initialized with a random value for use
// in testing STL map serialization
struct random_key {
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(
        Archive & ar, 
        const unsigned int /* file_version */
    ){
        ar & icecube::serialization::make_nvp("random_key", m_i);
    }
    int m_i;
    random_key() : m_i(std::rand()){};
    bool operator<(const random_key &rhs) const {
        return m_i < rhs.m_i;
    }
    bool operator==(const random_key &rhs) const {
        return m_i == rhs.m_i;
    }
    operator std::size_t () const {    // required by hash_map
        return m_i;
    }
};  

template <typename TS /*test settings*/>
void test_map(){
    auto testfile = I3Test::testfile("test_map");

    // test map of objects
    std::map<random_key, A> amap;
    amap.insert(std::make_pair(random_key(), A()));
    amap.insert(std::make_pair(random_key(), A()));
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("amap", amap);
    }
    std::map<random_key, A> amap1;
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("amap", amap1);
    }
    ENSURE(amap == amap1);
    std::remove(testfile.c_str());
}

template <typename TS /*test settings*/>
void test_map_2(){
    auto testfile = I3Test::testfile("test_map_2");

    std::pair<int, int> a(11, 22);
    std::map<int, int> b;
    b[0] = 0;
    b[-1] = -1;
    b[1] = 1;
    {
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        std::pair<int, int> * const pa = &a;
        std::map<int, int> * const pb = &b;
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << I3_SERIALIZATION_NVP(pb);
        oa << I3_SERIALIZATION_NVP(pa);
    }
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        std::pair<int, int> *pa = 0;
        std::map<int, int> *pb = 0;
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> I3_SERIALIZATION_NVP(pb);
        ia >> I3_SERIALIZATION_NVP(pa);
        ENSURE(a == *pa);
        ENSURE(b == *pb);
        delete pa;
        delete pb;
    }
    std::remove(testfile.c_str());
}

template <typename TS /*test settings*/>
void test_multimap(){
    auto testfile = I3Test::testfile("test_multimap");

    std::multimap<random_key, A> amultimap;
    amultimap.insert(std::make_pair(random_key(), A()));
    amultimap.insert(std::make_pair(random_key(), A()));
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("amultimap", amultimap);
    }
    std::multimap<random_key, A> amultimap1;
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("amultimap", amultimap1);
    }
    ENSURE(amultimap == amultimap1);
    std::remove(testfile.c_str());
}

#ifdef BOOST_HAS_HASH
#include <serialization/hash_map.hpp>

namespace BOOST_STD_EXTENSION_NAMESPACE {
    template<>
    struct hash<random_key>{
        std::size_t operator()(const random_key& r) const {
            return static_cast<std::size_t>(r);
        }
    };
} // namespace BOOST_STD_EXTENSION_NAMESPACE 

template <typename TS /*test settings*/>
void test_hash_map(){
    auto testfile = I3Test::testfile("test_hash_map");

    // test hash_map of objects
    BOOST_STD_EXTENSION_NAMESPACE::hash_map<random_key, A> ahash_map;
    ahash_map.insert(std::make_pair(random_key(), A()));
    ahash_map.insert(std::make_pair(random_key(), A()));
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("ahashmap",ahash_map);
    }
    BOOST_STD_EXTENSION_NAMESPACE::hash_map<random_key, A> ahash_map1;
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("ahashmap",ahash_map1);
    }

    std::vector< std::pair<random_key, A> > tvec, tvec1;
    std::copy(ahash_map.begin(), ahash_map.end(), std::back_inserter(tvec));
    std::sort(tvec.begin(), tvec.end());
    std::copy(ahash_map1.begin(), ahash_map1.end(), std::back_inserter(tvec1));
    std::sort(tvec1.begin(), tvec1.end());
    ENSURE(tvec == tvec1);

    std::remove(testfile.c_str());
}

template <typename TS /*test settings*/>
void test_hash_multimap(){
    auto testfile = I3Test::testfile("test_hash_multimap");

    BOOST_STD_EXTENSION_NAMESPACE::hash_multimap<random_key, A> ahash_multimap;
    ahash_multimap.insert(std::make_pair(random_key(), A()));
    ahash_multimap.insert(std::make_pair(random_key(), A()));
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("ahash_multimap", ahash_multimap);
    }
    BOOST_STD_EXTENSION_NAMESPACE::hash_multimap<random_key, A> ahash_multimap1;
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("ahash_multimap", ahash_multimap1);
    }
    std::vector< std::pair<random_key, A> > tvec, tvec1;
    tvec.clear();
    tvec1.clear();
    std::copy(ahash_multimap.begin(), ahash_multimap.end(), std::back_inserter(tvec));
    std::sort(tvec.begin(), tvec.end());
    std::copy(ahash_multimap1.begin(), ahash_multimap1.end(), std::back_inserter(tvec1));
    std::sort(tvec1.begin(), tvec1.end());
    ENSURE(tvec == tvec1);
    std::remove(testfile.c_str());
}
#endif

#include <serialization/unordered_map.hpp>
#include <functional> // requires changeset [69520]; Ticket #5254

namespace std {
    template<>
    struct hash<random_key>{
        std::size_t operator()(const random_key& r) const {
            return static_cast<std::size_t>(r);
        }
    };
} // namespace std

template <typename TS /*test settings*/>
void test_unordered_map(){
    auto testfile = I3Test::testfile("test_unordered_map");

    // test unordered_map of objects
    std::unordered_map<random_key, A> anunordered_map;
    anunordered_map.insert(std::make_pair(random_key(), A()));
    anunordered_map.insert(std::make_pair(random_key(), A()));
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("anunorderedmap",anunordered_map);
    }
    std::unordered_map<random_key, A> anunordered_map1;
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("anunorderedmap",anunordered_map1);
    }

    std::vector< std::pair<random_key, A> > tvec, tvec1;
    std::copy(anunordered_map.begin(), anunordered_map.end(), std::back_inserter(tvec));
    std::sort(tvec.begin(), tvec.end());
    std::copy(anunordered_map1.begin(), anunordered_map1.end(), std::back_inserter(tvec1));
    std::sort(tvec1.begin(), tvec1.end());
    ENSURE(tvec == tvec1);

    std::remove(testfile.c_str());
}

template <typename TS /*test settings*/>
void test_unordered_multimap(){
    auto testfile = I3Test::testfile("test_unordered_multimap");

    std::unordered_multimap<random_key, A> anunordered_multimap;
    anunordered_multimap.insert(std::make_pair(random_key(), A()));
    anunordered_multimap.insert(std::make_pair(random_key(), A()));
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("anunordered_multimap", anunordered_multimap);
    }
    std::unordered_multimap<random_key, A> anunordered_multimap1;
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("anunordered_multimap", anunordered_multimap1);
    }
    std::vector< std::pair<random_key, A> > tvec, tvec1;
    tvec.clear();
    tvec1.clear();
    std::copy(anunordered_multimap.begin(), anunordered_multimap.end(), std::back_inserter(tvec));
    std::sort(tvec.begin(), tvec.end());
    std::copy(anunordered_multimap1.begin(), anunordered_multimap1.end(), std::back_inserter(tvec1));
    std::sort(tvec1.begin(), tvec1.end());
    ENSURE(tvec == tvec1);
    std::remove(testfile.c_str());
}

TEST_GROUP(test_map)

#ifdef BOOST_HAS_HASH
    #define HASH_MAP_TEST(name) \
    TEST(name ## _hash_map){ \
        test_hash_map<test_settings>(); \
    } \
    TEST(name ## _hash_multimap){ \
        test_hash_multimap<test_settings>(); \
    }
#else
    #define HASH_MAP_TEST(name)
#endif

#define TEST_SET(name) \
TEST(name ## _map){ \
    test_map<test_settings>(); \
} \
TEST(name ## _map_2){ \
    test_map_2<test_settings>(); \
} \
TEST(name ## _multimap){ \
    test_multimap<test_settings>(); \
} \
HASH_MAP_TEST(name) \
TEST(name ## _unordered_map){ \
    test_unordered_map<test_settings>(); \
} \
TEST(name ## _unordered_multimap){ \
    test_unordered_multimap<test_settings>(); \
}

#define I3_ARCHIVE_TEST binary_archive.hpp
#include "select_archive.hpp"
TEST_SET(binary_archive)

#undef I3_ARCHIVE_TEST
#define I3_ARCHIVE_TEST text_archive.hpp
#include "select_archive.hpp"
TEST_SET(text_archive)

#undef I3_ARCHIVE_TEST
#define I3_ARCHIVE_TEST xml_archive.hpp
#include "select_archive.hpp"
TEST_SET(xml_archive)

#undef I3_ARCHIVE_TEST
#define I3_ARCHIVE_TEST portable_binary_archive.hpp
#include "select_archive.hpp"
TEST_SET(portable_binary_archive)
