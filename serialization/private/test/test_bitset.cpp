//  (C) Copyright 2009 Brian Ravnsgaard and Kenneth Riddile
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version including documentation.

// Test that serialization of std::bitset works.
// Should pass compilation and execution
// 16.09.2004, updated 04.03.2009

#include <I3Test.h>

#include <cstddef> // NULL
#include <cstdio> // remove
#include <fstream>

#include <boost/config.hpp>

#if defined( BOOST_NO_STDC_NAMESPACE )
namespace std
{ 
    using ::remove;
}
#endif

#include <serialization/bitset.hpp>
#include <serialization/nvp.hpp>
#include <serialization/dynamic_bitset.hpp>
#include <archive/xml_archive_exception.hpp>

//Serialize a bitset, followed by a string to ensure that subsequent objects
//are not corrupted.
template <std::size_t S /*bitset width*/, typename TS /*test settings*/>
void test_std_bitset_core(std::size_t V, std::size_t shift){
    std::cout << "Now testing " << S << " with pattern ";
    std::cout << std::hex << V << std::dec;
    std::cout << " and shift " << shift << std::endl;
    std::bitset<S> b(V);
    b <<= shift*8;
    std::string test("this is a test");
    
    auto testfile = I3Test::testfile("test_std_bitset_"+std::to_string(S));
    
    try{
        std::cout << " writing..." << std::endl;
        typename TS::test_ostream os( testfile, TS::TEST_STREAM_FLAGS );
        typename TS::test_oarchive oa( os, TS::TEST_ARCHIVE_FLAGS );
        oa << icecube::serialization::make_nvp( "bitset", b );
        oa << icecube::serialization::make_nvp( "test", test );
    }catch(...){
        std::cout << "write failed!" << std::endl;
        throw;
    }
    
    std::bitset<S> b_out;
    std::string test_out;
    try{
        std::cout << " reading..." << std::endl;
        typename TS::test_istream is( testfile, TS::TEST_STREAM_FLAGS );
        typename TS::test_iarchive ia( is, TS::TEST_ARCHIVE_FLAGS );
        ia >> icecube::serialization::make_nvp( "bitset", b_out );
        ia >> icecube::serialization::make_nvp( "test", test_out );
    }
    catch(icecube::archive::xml_archive_exception& ex){
        std::cout << "read failed with XML archive exception: " << ex.what() << std::endl;
        throw;
    }
    catch(...){
        std::cout << "read failed!" << std::endl;
        throw;
    }
    
    ENSURE(b == b_out);
    ENSURE(test == test_out);
    
    std::remove(testfile.c_str());
}

template<typename TS /*test settings*/, std::size_t max, std::size_t i=0>
struct IterateSizes : public IterateSizes<TS,max,i+1>{
    IterateSizes(){
        //reverse order to acocunt for subobject construction
        constexpr std::size_t ii=max-i;
        constexpr std::size_t value=(1U<<((ii%8)+1))-1;
        constexpr std::size_t shift=ii/8;
        test_std_bitset_core<ii+1,TS>(value,shift);
    }
};
template<typename TS /*test settings*/, std::size_t max>
struct IterateSizes<TS,max,max>{
    IterateSizes(){
        constexpr std::size_t ii=0;
        constexpr std::size_t value=(1U<<((ii%8)+1))-1;
        constexpr std::size_t shift=ii/8;
        test_std_bitset_core<ii+1,TS>(value,shift);
    }
};

template<typename BitsetT>
void set_some_bits(BitsetT& bits, unsigned int opt){
	ENSURE(bits.size()>3);
	ENSURE(opt<=3);
	switch(opt){
		case 3:
			bits.set(bits.size()-1);
			//fall through
		case 2:
			bits.set(3);
			//fall through
		case 1:
			bits.set(1);
			//fall through
		case 0:
			break;
	}
}

template<typename TS /*test settings*/, typename BitsetT>
void test_bitset(const BitsetT& bits_){
	std::cout << "Bitset size " << bits_.size() << std::endl;
    auto testfile = I3Test::testfile("test_std_bitset_"+std::to_string(bits_.size()));
	for(unsigned int j=0; j<4; j++){
		//make a bitset and set some bits in it
		BitsetT bits(bits_);
		set_some_bits(bits,j);
		std::cout << " initial bitset: " << bits << std::endl;
		//write out the bitset
		{
            typename TS::test_ostream os( testfile, TS::TEST_STREAM_FLAGS );
            typename TS::test_oarchive oa( os, TS::TEST_ARCHIVE_FLAGS );
            oa << icecube::serialization::make_nvp( "bitset", bits );
        }
		//read the bitset back
		BitsetT bits2;
		{
            typename TS::test_istream is( testfile, TS::TEST_STREAM_FLAGS );
            typename TS::test_iarchive ia( is, TS::TEST_ARCHIVE_FLAGS );
            ia >> icecube::serialization::make_nvp( "bitset", bits2 );
        }
		std::cout << " final bitset:   " << bits2 << std::endl;
		//check that all information was preserved
		ENSURE_EQUAL(bits,bits2);
	}
    std::remove(testfile.c_str());
}

template <typename TS /*test settings*/>
void test_std_bitset(){
    test_std_bitset_core<0,TS>(0,0);
    IterateSizes<TS,63> i;
    test_bitset<TS>(std::bitset<4>());
	test_bitset<TS>(std::bitset<8>());
	test_bitset<TS>(std::bitset<10>());
	test_bitset<TS>(std::bitset<23>());
	test_bitset<TS>(std::bitset<67>());
}

template <typename TS /*test settings*/>
void test_boost_dynamic_bitset(){
    test_bitset<TS>(boost::dynamic_bitset<>(4));
    test_bitset<TS>(boost::dynamic_bitset<>(8));
    test_bitset<TS>(boost::dynamic_bitset<>(10));
    test_bitset<TS>(boost::dynamic_bitset<>(23));
    test_bitset<TS>(boost::dynamic_bitset<>(67));
}


TEST_GROUP(bitset)

#define TEST_SET(name) \
TEST(name ## _std_bitset){ \
    test_std_bitset<test_settings>(); \
} \
TEST(name ## _boost_dynamic_bitset){ \
    test_boost_dynamic_bitset<test_settings>(); \
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
