/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_set.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// (C) Copyright 2014 Jim Bell
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

#include <cstddef> // NULLsize_t
#include <cstdio> // remove
#include <fstream>

#include <algorithm> // std::copy
#include <vector>

#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::size_t;
}
#endif

#include <boost/detail/workaround.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <archive/archive_exception.hpp>

#include <I3Test.h>

#include <serialization/nvp.hpp>
#include <serialization/set.hpp>

#include "A.hpp"
#include "A.ipp"

template <typename TS /*test settings*/>
void test_set(){
    auto testfile = I3Test::testfile("test_set");

    // test array of objects
    std::set<A> aset;
    aset.insert(A());
    aset.insert(A());
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("aset", aset);
    }
    std::set<A> aset1;
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("aset", aset1);
    }
    ENSURE(aset == aset1);
    std::remove(testfile.c_str());
}

template <typename TS /*test settings*/>
void test_multiset(){
    auto testfile = I3Test::testfile("test_multiset");

    std::multiset<A> amultiset;
    amultiset.insert(A());
    amultiset.insert(A());
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("amultiset", amultiset);
    }
    std::multiset<A> amultiset1;
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("amultiset", amultiset1);
    }
    ENSURE(amultiset == amultiset1);
    std::remove(testfile.c_str());
}

#ifdef BOOST_HAS_HASH

#include <serialization/hash_set.hpp>

namespace BOOST_STD_EXTENSION_NAMESPACE {
    template<>
    struct hash<A> {
        std::size_t operator()(const A& a) const {
            return static_cast<std::size_t>(a);
        }
    };
}

template <typename TS /*test settings*/>
void test_hash_set(){
    auto testfile = I3Test::testfile("test_hash_set");

    // test array of objects
    BOOST_STD_EXTENSION_NAMESPACE::hash_set<A> ahash_set;
    A a, a1;
    ahash_set.insert(a);
    ahash_set.insert(a1);
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("ahash_set", ahash_set);
    }
    BOOST_STD_EXTENSION_NAMESPACE::hash_set<A> ahash_set1;
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("ahash_set", ahash_set1);
    }
    std::vector<A> tvec, tvec1;
    tvec.clear();
    tvec1.clear();
    std::copy(ahash_set.begin(), ahash_set.end(), std::back_inserter(tvec));
    std::sort(tvec.begin(), tvec.end());
    std::copy(ahash_set1.begin(), ahash_set1.end(), std::back_inserter(tvec1));
    std::sort(tvec1.begin(), tvec1.end());
    ENSURE(tvec == tvec1);
    std::remove(testfile.c_str());
}

template <typename TS /*test settings*/>
void test_hash_multiset(){
    auto testfile = I3Test::testfile("test_hash_multiset");

    BOOST_STD_EXTENSION_NAMESPACE::hash_multiset<A> ahash_multiset;
    ahash_multiset.insert(A());
    ahash_multiset.insert(A());
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("ahash_multiset", ahash_multiset);
    }
    BOOST_STD_EXTENSION_NAMESPACE::hash_multiset<A> ahash_multiset1;
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("ahash_multiset", ahash_multiset1);
    }

    std::vector<A> tvec, tvec1;
    tvec.clear();
    tvec1.clear();
    std::copy(ahash_multiset.begin(), ahash_multiset.end(), std::back_inserter(tvec));
    std::sort(tvec.begin(), tvec.end());
    std::copy(ahash_multiset1.begin(), ahash_multiset1.end(), std::back_inserter(tvec1));
    std::sort(tvec1.begin(), tvec1.end());
    ENSURE(tvec == tvec1);

    std::remove(testfile.c_str());
}
#endif

#include <serialization/unordered_set.hpp>
#include <functional> // requires changeset [69520]; Ticket #5254

namespace std {
    template<>
    struct hash<A> {
        std::size_t operator()(const A& a) const {
            return static_cast<std::size_t>(a);
        }
    };
} // namespace std

template <typename TS /*test settings*/>
void test_unordered_set(){
    auto testfile = I3Test::testfile("test_unordered_set");

    // test array of objects
    std::unordered_set<A> anunordered_set;
    A a, a1;
    anunordered_set.insert(a);
    anunordered_set.insert(a1);
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("anunordered_set", anunordered_set);
    }
    std::unordered_set<A> anunordered_set1;
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("anunordered_set", anunordered_set1);
    }
    std::vector<A> tvec, tvec1;
    tvec.clear();
    tvec1.clear();
    std::copy(anunordered_set.begin(), anunordered_set.end(), std::back_inserter(tvec));
    std::sort(tvec.begin(), tvec.end());
    std::copy(anunordered_set1.begin(), anunordered_set1.end(), std::back_inserter(tvec1));
    std::sort(tvec1.begin(), tvec1.end());
    ENSURE(tvec == tvec1);
    std::remove(testfile.c_str());
}

template <typename TS /*test settings*/>
void test_unordered_multiset(){
    auto testfile = I3Test::testfile("test_unordered_multiset");

    std::unordered_multiset<A> anunordered_multiset;
    anunordered_multiset.insert(A());
    anunordered_multiset.insert(A());
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("anunordered_multiset", anunordered_multiset);
    }
    std::unordered_multiset<A> anunordered_multiset1;
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("anunordered_multiset", anunordered_multiset1);
    }

    std::vector<A> tvec, tvec1;
    tvec.clear();
    tvec1.clear();
    std::copy(anunordered_multiset.begin(), anunordered_multiset.end(), std::back_inserter(tvec));
    std::sort(tvec.begin(), tvec.end());
    std::copy(anunordered_multiset1.begin(), anunordered_multiset1.end(), std::back_inserter(tvec1));
    std::sort(tvec1.begin(), tvec1.end());
    ENSURE(tvec == tvec1);

    std::remove(testfile.c_str());
}

TEST_GROUP(test_map)

#ifdef BOOST_HAS_HASH
    #define HASH_SET_TEST(name) \
    TEST(name ## _hash_set){ \
        test_hash_set<test_settings>(); \
    } \
    TEST(name ## _hash_multiset){ \
        test_hash_multiset<test_settings>(); \
    }
#else
    #define HASH_SET_TEST(name)
#endif

#define TEST_SET(name) \
TEST(name ## _set){ \
    test_set<test_settings>(); \
} \
TEST(name ## _multiset){ \
    test_multiset<test_settings>(); \
} \
HASH_SET_TEST(name) \
TEST(name ## _unordered_set){ \
    test_unordered_set<test_settings>(); \
} \
TEST(name ## _unordered_multiset){ \
    test_unordered_multiset<test_settings>(); \
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
