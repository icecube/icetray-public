/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_diamond.cpp

// (C) Copyright 2002-2009 Vladimir Prus and Robert Ramey. 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// test of serialization library for diamond inheritence situations

#include <cstddef> // NULL
#include <fstream>
#include <iostream>

#include <boost/config.hpp>
#include <cstdio> // remove
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <I3Test.h>

#include <serialization/map.hpp>
#include <serialization/utility.hpp>
#include <serialization/split_member.hpp>
#include <serialization/tracking.hpp>
#include <serialization/base_object.hpp>
#include <serialization/nvp.hpp>
#include <serialization/export.hpp>

namespace{
int save_count = 0; // used to detect when base class is saved multiple times
int load_count = 0; // used to detect when base class is loaded multiple times
}

class base {
public:
    base() : i(0) {}
    base(int i) : i(i)
    {
        m[i] = "text";
    }

    template<class Archive>
    void save(Archive &ar, const unsigned int /* file_version */) const
    {
        std::cout << "Saving base\n";
        ar << I3_SERIALIZATION_NVP(i);
        ar << I3_SERIALIZATION_NVP(m);
        ++save_count;
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int /* file_version */)
    {
        std::cout << "Restoring base\n";
        ar >> I3_SERIALIZATION_NVP(i);
        ar >> I3_SERIALIZATION_NVP(m);
        ++load_count;
    }

    I3_SERIALIZATION_SPLIT_MEMBER()

    bool operator==(const base& another) const 
    {
        return i == another.i && m == another.m;
    }
    // make polymorphic by marking at least one function virtual
    virtual ~base() {};
private:
    int i;
    std::map<int, std::string> m;    
};

// note: the default is for object tracking to be performed if and only
// if and object of the corresponding class is anywhere serialized
// through a pointer.  In this example, that doesn't occur so 
// by default, the shared base object wouldn't normally be tracked.
// This would leave to multiple save/load operation of the data in
// this shared base class.  This wouldn't cause an error, but it would
// be a waste of time.  So set the tracking behavior trait of the base
// class to always track serialized objects of that class.  This permits
// the system to detect and elminate redundent save/load operations.
// (It is concievable that this might someday be detected automatically
// but for now, this is not done so we have to rely on the programmer
// to specify this trait)
I3_CLASS_TRACKING(base, track_always)

class derived1 : virtual public base {
public:
    template<class Archive>
    void save(Archive &ar, const unsigned int /* file_version */) const
    {
        std::cout << "Saving derived1\n";
        ar << I3_SERIALIZATION_BASE_OBJECT_NVP(base);
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int /* file_version */)
    {
        std::cout << "Restoring derived1\n";
        ar >> I3_SERIALIZATION_BASE_OBJECT_NVP(base);
    }

    I3_SERIALIZATION_SPLIT_MEMBER()
};

class derived2 : virtual public base {
public:    
    template<class Archive>
    void save(Archive &ar, const unsigned int /* file_version */) const
    {
        std::cout << "Saving derived2\n";
        ar << I3_SERIALIZATION_BASE_OBJECT_NVP(base);
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int /* file_version */)
    {
        std::cout << "Restoring derived2\n";
        ar >> I3_SERIALIZATION_BASE_OBJECT_NVP(base);
    }

    I3_SERIALIZATION_SPLIT_MEMBER()
};

class final : public derived1, public derived2 {
public:
    final() {}
    final(int i) : base(i) {}

    template<class Archive>
    void save(Archive &ar, const unsigned int /* file_version */) const
    {
        std::cout << "Saving final\n";
        ar << I3_SERIALIZATION_BASE_OBJECT_NVP(derived1);
        ar << I3_SERIALIZATION_BASE_OBJECT_NVP(derived2);
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int /* file_version */)
    {
        std::cout << "Restoring final\n";
        ar >> I3_SERIALIZATION_BASE_OBJECT_NVP(derived1);
        ar >> I3_SERIALIZATION_BASE_OBJECT_NVP(derived2);
    }

    I3_SERIALIZATION_SPLIT_MEMBER()
};

I3_CLASS_EXPORT(final)

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_diamond");
    
    const final b(3);   
    {
        typename TS::test_ostream ofs(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(ofs, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("b", b);
    }

    final b2;
    {
        typename TS::test_istream ifs(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(ifs, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("b2", b2);
    }
    ENSURE(1 == save_count);
    ENSURE(1 == load_count);
    ENSURE(b2 == b);
    std::remove(testfile.c_str());

    // do the same test with pointers

    save_count = 0;
    load_count = 0;

    const base* bp = new final( 3 );
    {
        typename TS::test_ostream ofs(testfile);
        typename TS::test_oarchive oa(ofs);
        oa << I3_SERIALIZATION_NVP(bp);
    }

    base* bp2;
    {
        typename TS::test_istream ifs(testfile);
        typename TS::test_iarchive ia(ifs);
        ia >> I3_SERIALIZATION_NVP(bp2);
    }

    ENSURE(1 == save_count);
    ENSURE(1 == load_count);
    ENSURE(*bp2 == *bp);
    std::remove(testfile.c_str());
}

TEST_GROUP(test_diamond)

#define TEST_SET(name) \
TEST(name ## _test_diamond){ \
    do_test<test_settings>(); \
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
