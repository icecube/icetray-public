/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_private_base.cpp

// (C) Copyright 2009 Eric Moyer - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

// invoke header for a custom archive test.

#include <fstream>
#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <serialization/access.hpp>
#include <serialization/base_object.hpp>
#include <serialization/export.hpp>
#include <serialization/nvp.hpp>

#include <I3Test.h>

class Base {
    friend class icecube::serialization::access;
    int m_i;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version){
        ar & m_i;
    }
protected:
    bool equals(const Base &rhs) const {
        return m_i == rhs.m_i;
    }
    Base(int i = 0) :
        m_i(i)
    {}
    virtual ~Base(){};
public:
    virtual bool operator==(const Base &rhs) const {
        return false;
    }// = 0;
};

class Derived :  private Base {
    friend class icecube::serialization::access;
private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version){
        ar & icecube::serialization::base_object<Base>(*this);
    }
public:
    virtual bool operator==(const Derived &rhs) const {
        return Base::equals(static_cast<const Base &>(rhs));
    }
    Derived(int i = 0) :
        Base(i)
    {}
    static Base* make_base(int i=0){
        return((Base*)new Derived(i));
    }
};

I3_CLASS_EXPORT(Derived)

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_private_base");

    Derived a(1), a1(2);
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("a", a);
    }
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("a", a1);
    }
    ENSURE(a == a1);
    std::remove(testfile.c_str());

    Base* ta = Derived::make_base(1);
    Base* ta1 = NULL;
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
    ENSURE(*dynamic_cast<Derived*>(ta) == *dynamic_cast<Derived*>(ta1));
    
    std::remove(testfile.c_str());
}
