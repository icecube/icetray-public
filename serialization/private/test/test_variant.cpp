/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_variant.cpp
// test of non-intrusive serialization of variant types
//
// copyright (c) 2005   
// troy d. straszheim <troy@resophonic.com>
// http://www.resophonic.com
//
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org for updates, documentation, and revision history.
//
// thanks to Robert Ramey and Peter Dimov.
//

#include <cstddef> // NULL
#include <cstdio> // remove
#include <fstream>
#include <boost/config.hpp>
#include <boost/math/special_functions/next.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <serialization/throw_exception.hpp>

#if defined(_MSC_VER) && (_MSC_VER <= 1020)
#  pragma warning (disable : 4786) // too long name, harmless warning
#endif

#include <I3Test.h>

#include <archive/archive_exception.hpp>

#include <serialization/nvp.hpp>
#include <serialization/variant.hpp>

#include "A.hpp"
#include "A.ipp"

class are_equal
    : public boost::static_visitor<bool>
{
public:
    // note extra rigamorole for compilers which don't support
    // partial function template ordering - specfically msvc 6.x
    struct same {
        template<class T, class U>
        static bool invoke(const T & t, const U & u){
            return t == u;
        }
    };

    struct not_same {
        template<class T, class U>
        static bool invoke(const T &, const U &){
            return false;
        }
    };

    template <class T, class U>
    bool operator()( const T & t, const U & u) const 
    {
        typedef typename boost::mpl::eval_if<boost::is_same<T, U>,
            boost::mpl::identity<same>,
            boost::mpl::identity<not_same>
        >::type type;
        return type::invoke(t, u);
    }

    bool operator()( const float & lhs, const float & rhs ) const
    {
        return std::abs( boost::math::float_distance(lhs, rhs)) < 2;
    }
    bool operator()( const double & lhs, const double & rhs ) const
    {
        return std::abs( boost::math::float_distance(lhs, rhs)) < 2;
    }
};

template <typename TS /*test settings*/, class T>
void test_type(const T& gets_written){
   auto testfile = I3Test::testfile("variant_test_type");
   {
      typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
      typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
      oa << icecube::serialization::make_nvp("written", gets_written);
   }

   T got_read;
   {
      typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
      typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
      ia >> icecube::serialization::make_nvp("written", got_read);
   }
   ENSURE(boost::apply_visitor(are_equal(), gets_written, got_read));

   std::remove(testfile.c_str());
}

// this verifies that if you try to read in a variant from a file
// whose "which" is illegal for the one in memory (that is, you're
// reading in to a different variant than you wrote out to) the load()
// operation will throw.  One could concievably add checking for
// sequence length as well, but this would add size to the archive for
// dubious benefit.
//
template <typename TS /*test settings*/>
void do_bad_read()
{
    // Compiling this test causes an ICE on msvc 6
    // So, we'll just to skip it for this compiler
    #if defined(_MSC_VER) && (_MSC_VER <= 1020)
        boost::variant<bool, float, int, std::string> big_variant;
        big_variant = std::string("adrenochrome");

        auto testfile = I3Test::testfile("variant_bad_read");
        {
            typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
            typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
            oa << I3_SERIALIZATION_NVP(big_variant);
        }
        boost::variant<bool, float, int> little_variant;
        {
            typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
            typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
            bool exception_invoked = false;
            try{
                ia >> I3_SERIALIZATION_NVP(little_variant);
            }catch(icecube::archive::archive_exception e) {
                ENSURE(icecube::archive::archive_exception::unsupported_version == e.code);
                exception_invoked = true;
            }
            ENSURE(exception_invoked);
        }
    #endif
}

TEST_GROUP(variant)

#define TEST_SET(name) \
TEST(name ## _boost_variant){ \
   { \
      boost::variant<bool, int, float, double, A, std::string> v; \
      v = false; \
      test_type<test_settings>(v); \
      v = 1; \
      test_type<test_settings>(v); \
      v = (float) 2.3; \
      test_type<test_settings>(v); \
      v = (double) 6.4; \
      test_type<test_settings>(v); \
      v = std::string("we can't stop here, this is Bat Country"); \
      test_type<test_settings>(v); \
      v = A(); \
      test_type<test_settings>(v); \
   } \
   do_bad_read<test_settings>(); \
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
