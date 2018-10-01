/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_new_operator.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

#include <cstddef> // NULL
#include <cstdio> // remove
#include <fstream>
#include <new>

#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <serialization/access.hpp>

#include <I3Test.h>

#include "A.hpp"
#include "A.ipp"

class ANew : public A {
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned /*file_version*/){
        ar & I3_SERIALIZATION_BASE_OBJECT_NVP(A);
    }
public:
    static unsigned int m_new_calls;
    static unsigned int m_delete_calls;
    // implement class specific new/delete in terms standard
    // implementation - we're testing serialization 
    // not "new" here.
    static void* operator new(size_t s){
        ++m_new_calls;
        return ::operator new(s);
    }
    static void operator delete(void* p, std::size_t /*s*/){
        ++m_delete_calls;
        ::operator delete(p);
    }
};

unsigned int ANew::m_new_calls = 0;
unsigned int ANew::m_delete_calls = 0;

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_mi");

    ANew *ta = new ANew();

    ENSURE(1 == ANew::m_new_calls);
    ENSURE(0 == ANew::m_delete_calls);

    ANew* ta1 = NULL;

    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("ta", ta);
    }
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("ta", ta1);
    }
    ENSURE(ta != ta1);
    ENSURE(*ta == *ta1);

    ENSURE(2 == ANew::m_new_calls);
    ENSURE(0 == ANew::m_delete_calls);

    std::remove(testfile.c_str());

    delete ta;
    delete ta1;

    ENSURE(2 == ANew::m_new_calls);
    ENSURE(2 == ANew::m_delete_calls);
}

// EOF
