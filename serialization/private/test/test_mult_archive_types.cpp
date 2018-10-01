/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_mult_archive_types.cpp 

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstddef>
#include <fstream>

#include <boost/config.hpp>
#include <cstdio> // remove
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <archive/binary_oarchive.hpp>
#include <archive/binary_iarchive.hpp>
#include <archive/text_oarchive.hpp>
#include <archive/text_iarchive.hpp>
#include <archive/xml_oarchive.hpp>
#include <archive/xml_iarchive.hpp>
#include <archive/portable_binary_archive.hpp>
#include <I3Test.h>

#include <serialization/export.hpp>
#include <serialization/nvp.hpp>

namespace{
// This is a simple class.  It contains a counter of the number
// of objects of this class which have been instantiated.
class A
{
private:
    friend class icecube::serialization::access;
    int x;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & I3_SERIALIZATION_NVP(x);
    }
public:
    static int count;
    A(){++count;}    // default constructor
    virtual ~A(){--count;}   // default destructor
};
}

I3_CLASS_EXPORT(A)

namespace{
// B is a subclass of A
class B : public A
{
private:
    friend class icecube::serialization::access;
    int y;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & I3_SERIALIZATION_BASE_OBJECT_NVP(A);
    }
public:
    static int count;
    B() : A() {};
    virtual ~B() {};
};
}

I3_CLASS_EXPORT(B)

namespace{
int A::count = 0;
}

// Run tests by serializing two shared_ptrs into an archive of type
// OARCH, clearing them (deleting the objects) and then reloading the
// objects back from an archive of type OARCH.
template<class OA, class IA>
void test_save_and_load(A * first, A * second)
{
    auto testfile = I3Test::testfile("mult_archive_types");

    // Save
    {
        std::ofstream os(testfile);
        OA oa(os);
        oa << I3_SERIALIZATION_NVP(first);
        oa << I3_SERIALIZATION_NVP(second);
    }

    // Clear the pointers, thereby destroying the objects they contain
    first = nullptr;
    second = nullptr;

    // Load
    {
        std::ifstream is(testfile);
        IA ia(is);
        ia >> I3_SERIALIZATION_NVP(first);
        ia >> I3_SERIALIZATION_NVP(second);
    }
    ENSURE(first == second);
    std::remove(testfile.c_str());
}

using namespace icecube::archive;

TEST_GROUP(mult_archive_types)

TEST(binary_archive){
    // Try to save and load pointers to As, to a binary archive
    A* a = new A;
    A* a1 = a;
    test_save_and_load<binary_oarchive, binary_iarchive>(a, a1);

    // Try to save and load pointers to Bs, to a binary archive
    B* b = new B;
    B* b1 = b;
    test_save_and_load<binary_oarchive, binary_iarchive>(b, b1);
}

TEST(text_archive){
    // Try to save and load pointers to As, to a text archive
    A* a = new A;
    A* a1 = a;
    test_save_and_load<text_oarchive, text_iarchive>(a, a1);

    // Try to save and load pointers to Bs, to a text archive
    B* b = new B;
    B* b1 = b;
    test_save_and_load<text_oarchive, text_iarchive>(b, b1);
}

TEST(xml_archive){
    // Try to save and load pointers to As, to a xml archive
    A* a = new A;
    A* a1 = a;
    test_save_and_load<xml_oarchive, xml_iarchive>(a, a1);

    // Try to save and load pointers to Bs, to a xml archive
    B* b = new B;
    B* b1 = b;
    test_save_and_load<xml_oarchive, xml_iarchive>(b, b1);
}

TEST(portable_binary_archive){
    // Try to save and load pointers to As, to a portable binary archive
    A* a = new A;
    A* a1 = a;
    test_save_and_load<portable_binary_oarchive, portable_binary_iarchive>(a, a1);
    
    // Try to save and load pointers to Bs, to a portable binary archive
    B* b = new B;
    B* b1 = b;
    test_save_and_load<portable_binary_oarchive, portable_binary_iarchive>(b, b1);
}
