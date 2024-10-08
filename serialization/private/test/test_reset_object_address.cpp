/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_reset_object_address.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// SPDX-License-Identifier: BSL-1.0
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <sstream>
#include <cassert>
#include <cstdlib> // for rand()
#include <cstddef> // size_t

#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::rand;
    using ::size_t;
}
#endif

#include <I3Test.h>

#include <archive/text_oarchive.hpp>
#include <archive/text_iarchive.hpp>
#include <archive/polymorphic_text_oarchive.hpp>
#include <archive/polymorphic_text_iarchive.hpp>

#include <serialization/list.hpp>
#include <serialization/access.hpp>

// Someday, maybe all tests will be converted to the unit test framework.
// but for now use the text execution monitor to be consistent with all
// the other tests.

// simple test of untracked value
#include "A.hpp"
#include "A.ipp"

TEST_GROUP(reset_object_address)

TEST(test1){
    std::stringstream ss;
    const A a;
    {
        icecube::archive::text_oarchive oa(ss);
        oa << a;
    }
    A a1;
    {
        icecube::archive::text_iarchive ia(ss);
        // load to temporary
        A a2;
        ia >> a2;
        ENSURE(a==a2);
        // move to final destination
        a1 = a2;
        ia.reset_object_address(& a1, & a2);
    }
    ENSURE(a==a1);
}

namespace{
// simple test of tracked value
class B {
    friend class icecube::serialization::access;
    int m_i;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int /*file_version*/){
        ar & m_i;
    }
public:
    bool operator==(const B & rhs) const {
        return m_i == rhs.m_i;
    }
    B() :
        m_i(std::rand())
    {}
};
}

TEST(test2){
    std::stringstream ss;
    B const b;
    B const* const b_ptr = & b;
    ENSURE_EQUAL(&b, b_ptr);
    {
        icecube::archive::text_oarchive oa(ss);
        oa << b;
        oa << b_ptr;
    }
    B b1;
    B* b1_ptr;
    {
        icecube::archive::text_iarchive ia(ss);
        // load to temporary
        B b2;
        ia >> b2;
        ENSURE(b==b2);
        // move to final destination
        b1 = b2;
        ia.reset_object_address(&b1, &b2);
        ia >> b1_ptr;
    }
    ENSURE(b==b1);
    ENSURE_EQUAL(&b1, b1_ptr);
}

namespace{
// check that nested member values are properly moved
class D {
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int /*file_version*/){
        ar & m_b;
    }
public:
    B m_b;
    bool operator==(const D & rhs) const {
        return m_b == rhs.m_b;
    }
    D(){}
};
}

TEST(test3){
    std::stringstream ss;
    D const d;
    B const * const b_ptr = & d.m_b;
    {
        icecube::archive::text_oarchive oa(ss);
        oa << d;
        oa << b_ptr;
    }
    D d1;
    B* b1_ptr;
    {
        icecube::archive::text_iarchive ia(ss);
        D d2;
        ia >> d2;
        d1 = d2;
        ia.reset_object_address(&d1, &d2);
        ia >> b1_ptr;
    }
    ENSURE(d==d1);
    ENSURE(*b_ptr==*b1_ptr);
}

namespace{
// check that data pointed to by pointer members is NOT moved
class E {
    int m_i;
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int /*file_version*/){
        ar & m_i;
    }
public:
    bool operator==(const E &rhs) const {
        return m_i == rhs.m_i;
    }
    E() :
        m_i(std::rand())
    {}
    E(const E & rhs) :
        m_i(rhs.m_i)
    {}
};

// check that moves don't move stuff pointed to
class F {
    friend class icecube::serialization::access;
    E * m_eptr;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int /*file_version*/){
        ar & m_eptr;
    }
public:
    bool operator==(const F &rhs) const {
        return *m_eptr == *rhs.m_eptr;
    }
    F & operator=(const F & rhs) {
        * m_eptr = * rhs.m_eptr;
        return *this;
    }
    F(){
        m_eptr = new E;
    }
    F(const F & rhs){
        *this = rhs;
    }
    ~F(){
        delete m_eptr;
    }
};
}

TEST(test4){
    std::stringstream ss;
    const F f;
    {
        icecube::archive::text_oarchive oa(ss);
        oa << f;
    }
    F f1;
    {
        icecube::archive::text_iarchive ia(ss);
        F f2;
        ia >> f2;
        f1 = f2;
        ia.reset_object_address(& f1, & f2);
    }
    ENSURE(f==f1);
}

namespace{
// check that multiple moves keep track of the correct target
class G {
    friend class icecube::serialization::access;
    A m_a1;
    A m_a2;
    A *m_pa2;
    template<class Archive>
    void save(Archive &ar, const unsigned int /*file_version*/) const {
        ar << m_a1;
        ar << m_a2;
        ar << m_pa2;
    }
    template<class Archive>
    void load(Archive &ar, const unsigned int /*file_version*/){
        A a; // temporary A
        ar >> a;
        m_a1 = a;
        ar.reset_object_address(& m_a1, & a);
        ar >> a;
        m_a2 = a;
        ar.reset_object_address(& m_a2, & a);
        ar & m_pa2;
    }
    I3_SERIALIZATION_SPLIT_MEMBER()
public:
    bool operator==(const G &rhs) const {
        return
            m_a1 == rhs.m_a1
            && m_a2 == rhs.m_a2
            && *m_pa2 == *rhs.m_pa2;
    }
    G & operator=(const G & rhs) {
        m_a1 = rhs.m_a1;
        m_a2 = rhs.m_a2;
        m_pa2 = & m_a2;
        return *this;
    }
    G(){
        m_pa2 = & m_a2;
    }
    G(const G & rhs){
        *this = rhs;
    }
    ~G(){}
};
}

TEST(test5){
    std::stringstream ss;
    const G g;
    {
        icecube::archive::text_oarchive oa(ss);
        oa << g;
    }
    G g1;
    {
        icecube::archive::text_iarchive ia(ss);
        ia >> g1;
    }
    ENSURE(g==g1);
}

// joaquin's test - this tests the case where reset_object_address
// is applied to an item which in fact is not tracked so that
// the call is in fact superfluous.
struct foo
{
  int x;

private:
  friend class icecube::serialization::access;

  template<class Archive>
  void serialize(Archive &,const unsigned int){}
};

struct bar
{
  foo  f[2];
  foo* pf[2];

private:
  friend class icecube::serialization::access;
  I3_SERIALIZATION_SPLIT_MEMBER()

  template<class Archive>
  void save(Archive& ar,const unsigned int)const
  {
    for(int i=0;i<2;++i){
      ar<<f[i].x;
      ar<<f[i];
    }
    for(int j=0;j<2;++j){
      ar<<pf[j];
    }
  }

  template<class Archive>
  void load(Archive& ar,const unsigned int)
  {
    for(int i=0;i<2;++i){
      int x;
      ar>>x;
      f[i].x=x;
      ar.reset_object_address(&f[i].x,&x);
      ar>>f[i];
    }
    for(int j=0;j<2;++j){
      ar>>pf[j];
    }
  }
};

TEST(test6){
  bar b;
  b.f[0].x=0;
  b.f[1].x=1;
  b.pf[0]=&b.f[0];
  b.pf[1]=&b.f[1];

  std::ostringstream oss;
  {
    icecube::archive::text_oarchive oa(oss);
    oa<<const_cast<const bar&>(b);
  }

  bar b1;
  b1.pf[0]=0;
  b1.pf[1]=0;

  std::istringstream iss(oss.str());
  icecube::archive::text_iarchive ia(iss);
  ia>>b1;
  ENSURE(b1.pf[0]==&b1.f[0]&&b1.pf[1]==&b1.f[1]);
}

// test one of the collections
TEST(test7){
    std::stringstream ss;
    B const b;
    B const* const b_ptr = & b;
    ENSURE_EQUAL(& b, b_ptr);
    {
        std::list<const B *> l;
        l.push_back(b_ptr);
        icecube::archive::text_oarchive oa(ss);
        oa << const_cast<const std::list<const B *> &>(l);
    }
    B b1;
    {
        std::list<B *> l;
        icecube::archive::text_iarchive ia(ss);
        ia >> l;
        delete l.front(); // prevent memory leak
    }
}

// test one of the collections with polymorphic archive
TEST(test8){
    std::stringstream ss;
    B const b;
    B const* const b_ptr = & b;
    ENSURE_EQUAL(& b, b_ptr);
    {
        std::list<const B*> l;
        l.push_back(b_ptr);
        icecube::archive::polymorphic_text_oarchive oa(ss);
        icecube::archive::polymorphic_oarchive & poa = oa;
        poa << const_cast<const std::list<const B *> &>(l);
    }
    B b1;
    {
        std::list<B*> l;
        icecube::archive::polymorphic_text_iarchive ia(ss);
        icecube::archive::polymorphic_iarchive & pia = ia;
        pia >> l;
        delete l.front(); // prevent memory leak
    }
}
