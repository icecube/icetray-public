/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_delete_pointer.cpp

// (C) Copyright 2002 Vahan Margaryan. 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstddef> // NULL
#include <fstream>

#include <cstdio> // remove
#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <archive/archive_exception.hpp>
#include <serialization/throw_exception.hpp>
#include <serialization/nvp.hpp>
#include <serialization/split_member.hpp>

#include <I3Test.h>

namespace{
//A holds a pointer to another A, but doesn't own the pointer.
//objCount
class A
{
    friend class icecube::serialization::access;
    template<class Archive>
    void save(Archive &ar, const unsigned int /* file_version */) const
    {
        ar << I3_SERIALIZATION_NVP(next_);
    }
    template<class Archive>
    void load(Archive & ar, const unsigned int /* file_version */)
    {
        static int i = 0;
        ar >> I3_SERIALIZATION_NVP(next_);
        //if(++i == 3)
        //    icecube::serialization::throw_exception(icecube::archive::archive_exception(
        //        icecube::archive::archive_exception::no_exception
        //    ));
        ++loadcount;
    }
    I3_SERIALIZATION_SPLIT_MEMBER()
public:
    A()
    {
        if(test && objcount == 3)
            icecube::serialization::throw_exception(icecube::archive::archive_exception(
                icecube::archive::archive_exception::no_exception
            ));
        next_ = 0;
        ++objcount;
    }
    ~A(){
        delete next_;
        --objcount;
    }
    A* next_;
    static int objcount;
    static bool test;
    static int loadcount;
};


int A::objcount = 0;
int A::loadcount = 0;
bool A::test = false;
}

template <typename TS /*test settings*/>
void do_test(){
    //fill the vector with chained A's. The vector is assumed
    //to own the objects - we will destroy the objects through this vector.

    A* head = new A;
    A* last = head;
    unsigned int i;
    for(i = 1; i < 9; ++i){
        A* a = new A;
        last->next_ = a;
        last = a;
    }

    auto testfile = I3Test::testfile("test_delete_pointer");

    //output the list
    {
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << I3_SERIALIZATION_NVP(head);
    }

    delete head;
    ENSURE(A::objcount == 0);

    head = NULL;
    A::test = true;
    //read the list back
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        try{
            ia >> I3_SERIALIZATION_NVP(head);
        }
        catch(...){
            ia.delete_created_pointers();
        }
    }

    //identify the leaks
    ENSURE(A::loadcount == 0);
    std::remove(testfile.c_str());
}

TEST_GROUP(test_delete_pointer)

#define TEST_SET(name) \
TEST(name ## _delete_pointer){ \
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
