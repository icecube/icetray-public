/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_helper_support.cpp

// (C) Copyright 2008 Joaquin M Lopez Munoz. 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

#include <algorithm>
#include <cstddef>
#include <fstream>
#include <string>
#include <vector>

#include <cstdio> // remove
#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <I3Test.h>
#include <boost/lexical_cast.hpp>
#include <serialization/split_free.hpp>
#include <serialization/vector.hpp>

class my_string:public std::string
{
    typedef std::string super;

public:
    my_string(){}
    my_string(const super & str): super(str){}
    my_string & operator=(const super& rhs) {
      super::operator=(rhs);
      return *this;
    }
};

struct my_string_helper
{
  typedef std::vector<my_string> table;
  table m_t;
};

I3_SERIALIZATION_SPLIT_FREE(my_string)

namespace icecube {
namespace serialization {

template<class Archive>
void save(Archive & ar, const my_string & str, const unsigned int /* version */)
{
    void (* const idx)(Archive &, const my_string &, const unsigned int) = & save;
    void * const id = reinterpret_cast<void * const>(idx);
    my_string_helper & msh = ar.template get_helper<my_string_helper>(id);

    my_string_helper::table t = msh.m_t;
    my_string_helper::table::iterator it = std::find(t.begin(), t.end(), str);
    if(it == t.end()){
        my_string_helper::table::size_type s = t.size();
        ar << make_nvp("index", s);
        t.push_back(str);
        ar << make_nvp("string", static_cast<const std::string &>(str));
    }
    else{
        my_string_helper::table::size_type s = it - t.begin();
        ar << make_nvp("index", s);
    }
}

template<class Archive>
void load(Archive & ar, my_string & str, const unsigned int /* version */)
{
    void (* const idx)(Archive &, my_string &, const unsigned int) = & load;
    void * const id = reinterpret_cast<void * const>(idx);
    my_string_helper & msh = ar.template get_helper<my_string_helper>(id);

    my_string_helper::table t = msh.m_t;

    my_string_helper::table::size_type s;
    ar >> make_nvp("index", s);
    t.reserve(s);
    if(s >= t.size()){
        std::string tmp;
        ar >> make_nvp("string", tmp);
        str = tmp;
        t.push_back(str);
    }
    else{
        str = t[s];
    }
}

} // namespace serialization
} // namespace icecube

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_complex");

    std::vector<my_string> v1;
    for(int i=0; i<1000; ++i){
        v1.push_back(my_string(boost::lexical_cast<std::string>(i % 100)));
    }
    {
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("vector", v1);
    }
    {
        std::vector<my_string> v2;
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("vector", v2);
        ENSURE(v1 == v2);
    }
    std::remove(testfile.c_str());
}

TEST_GROUP(test_helper_support)

#define TEST_SET(name) \
TEST(name ## _helper_support){ \
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

// EOF
