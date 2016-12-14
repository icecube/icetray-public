/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_diamond.cpp

// (C) Copyright 2002-2009 Vladimir Prus, Robert Ramey and Takatoshi Kondo.
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

#include <serialization/map.hpp>
#include <serialization/utility.hpp>
#include <serialization/split_member.hpp>
#include <serialization/tracking.hpp>
#include <serialization/base_object.hpp>
#include <serialization/nvp.hpp>
#include <serialization/export.hpp>

#include <I3Test.h>

namespace{
int save_count = 0; // used to detect when EXnLevel1 class is saved multiple times
int load_count = 0; // used to detect when EXnLevel1 class is loaded multiple times
}

// inheritance structure
//
// EX1Level1<-+-EX1Level2_A<-+-+-EX1Level3_A
//            |              | |
//            +-EX1Level2_B<-+ +-EX1Level3_B<--EX1Level4
//
// EXPORT Sequence EX1Level3_A, EX1Level4
//---------------------------------------------------------
// EX2Level1<-+-EX2Level2_A<-+-+-EX2Level3_A
//            |              | |
//            +-EX2Level2_B<-+ +-EX2Level3_B<--EX2Level4
//
// EXPORT Sequence EX2Level4, EX2Level3_A

class EX1Level1 {
public:
    EX1Level1() : i(0) {}
    EX1Level1(int i) : i(i)
    {
        m[i] = "text";
    }

    template<class Archive>
    void save(Archive &ar, const unsigned int /* file_version */) const
    {
        std::cout << "Saving EX1Level1\n";
        ar << I3_SERIALIZATION_NVP(i);
        ar << I3_SERIALIZATION_NVP(m);
        ++save_count;
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int /* file_version */)
    {
        std::cout << "Restoring EX1Level1\n";
        ar >> I3_SERIALIZATION_NVP(i);
        ar >> I3_SERIALIZATION_NVP(m);
        ++load_count;
    }

    I3_SERIALIZATION_SPLIT_MEMBER()

    bool operator==(const EX1Level1& another) const 
    {
        return i == another.i && m == another.m;
    }
    // make polymorphic by marking at least one function virtual
    virtual ~EX1Level1() {};
private:
    int i;
    std::map<int, std::string> m;    
};

// note: the default is for object tracking to be performed if and only
// if and object of the corresponding class is anywhere serialized
// through a pointer.  In this example, that doesn't occur so 
// by default, the shared EX1Level1 object wouldn't normally be tracked.
// This would leave to multiple save/load operation of the data in
// this shared EX1Level1 class.  This wouldn't cause an error, but it would
// be a waste of time.  So set the tracking behavior trait of the EX1Level1
// class to always track serialized objects of that class.  This permits
// the system to detect and elminate redundent save/load operations.
// (It is concievable that this might someday be detected automatically
// but for now, this is not done so we have to rely on the programmer
// to specify this trait)
I3_CLASS_TRACKING(EX1Level1, track_always)

class EX1Level2_A : virtual public EX1Level1 {
public:
    template<class Archive>
    void save(Archive &ar, const unsigned int /* file_version */) const
    {
        std::cout << "Saving EX1Level2_A\n";
        ar << I3_SERIALIZATION_BASE_OBJECT_NVP(EX1Level1);
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int /* file_version */)
    {
        std::cout << "Restoring EX1Level2_A\n";
        ar >> I3_SERIALIZATION_BASE_OBJECT_NVP(EX1Level1);
    }

    I3_SERIALIZATION_SPLIT_MEMBER()
};

class EX1Level2_B : virtual public EX1Level1 {
public:    
    template<class Archive>
    void save(Archive &ar, const unsigned int /* file_version */) const
    {
        std::cout << "Saving EX1Level2_B\n";
        ar << I3_SERIALIZATION_BASE_OBJECT_NVP(EX1Level1);
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int /* file_version */)
    {
        std::cout << "Restoring EX1Level2_B\n";
        ar >> I3_SERIALIZATION_BASE_OBJECT_NVP(EX1Level1);
    }

    I3_SERIALIZATION_SPLIT_MEMBER()
};

class EX1Level3_A : public EX1Level2_A, public EX1Level2_B {
public:
    EX1Level3_A() {}
    EX1Level3_A(int i) : EX1Level1(i) {}

    template<class Archive>
    void save(Archive &ar, const unsigned int /* file_version */) const
    {
        std::cout << "Saving EX1Level3_A\n";
        ar << I3_SERIALIZATION_BASE_OBJECT_NVP(EX1Level2_A);
        ar << I3_SERIALIZATION_BASE_OBJECT_NVP(EX1Level2_B);
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int /* file_version */)
    {
        std::cout << "Restoring EX1Level3_A\n";
        ar >> I3_SERIALIZATION_BASE_OBJECT_NVP(EX1Level2_A);
        ar >> I3_SERIALIZATION_BASE_OBJECT_NVP(EX1Level2_B);
    }

    I3_SERIALIZATION_SPLIT_MEMBER()
};


class EX1Level3_B : public EX1Level2_A, public EX1Level2_B {
public:
    EX1Level3_B() {}
    EX1Level3_B(int) {}

    template<class Archive>
    void save(Archive &ar, const unsigned int /* file_version */) const
    {
        std::cout << "Saving EX1Level3_B\n";
        ar << I3_SERIALIZATION_BASE_OBJECT_NVP(EX1Level2_A);
        ar << I3_SERIALIZATION_BASE_OBJECT_NVP(EX1Level2_B);
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int /* file_version */)
    {
        std::cout << "Restoring EX1Level3_B\n";
        ar >> I3_SERIALIZATION_BASE_OBJECT_NVP(EX1Level2_A);
        ar >> I3_SERIALIZATION_BASE_OBJECT_NVP(EX1Level2_B);
    }

    I3_SERIALIZATION_SPLIT_MEMBER()
};

class EX1Level4 : public EX1Level3_B {
public:
    EX1Level4() {}
    EX1Level4(int i) : EX1Level1(i) {}

    template<class Archive>
    void save(Archive &ar, const unsigned int /* file_version */) const
    {
        std::cout << "Saving EX1Level4\n";
        ar << I3_SERIALIZATION_BASE_OBJECT_NVP(EX1Level3_B);
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int /* file_version */)
    {
        std::cout << "Restoring EX1Level4\n";
        ar >> I3_SERIALIZATION_BASE_OBJECT_NVP(EX1Level3_B);
    }

    I3_SERIALIZATION_SPLIT_MEMBER()
};


class EX2Level1 {
public:
    EX2Level1() : i(0) {}
    EX2Level1(int i) : i(i)
    {
        m[i] = "text";
    }

    template<class Archive>
    void save(Archive &ar, const unsigned int /* file_version */) const
    {
        std::cout << "Saving EX2Level1\n";
        ar << I3_SERIALIZATION_NVP(i);
        ar << I3_SERIALIZATION_NVP(m);
        ++save_count;
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int /* file_version */)
    {
        std::cout << "Restoring EX2Level1\n";
        ar >> I3_SERIALIZATION_NVP(i);
        ar >> I3_SERIALIZATION_NVP(m);
        ++load_count;
    }

    I3_SERIALIZATION_SPLIT_MEMBER()

    bool operator==(const EX2Level1& another) const 
    {
        return i == another.i && m == another.m;
    }
    // make polymorphic by marking at least one function virtual
    virtual ~EX2Level1() {};
private:
    int i;
    std::map<int, std::string> m;    
};

// note: the default is for object tracking to be performed if and only
// if and object of the corresponding class is anywhere serialized
// through a pointer.  In this example, that doesn't occur so 
// by default, the shared EX2Level1 object wouldn't normally be tracked.
// This would leave to multiple save/load operation of the data in
// this shared EX2Level1 class.  This wouldn't cause an error, but it would
// be a waste of time.  So set the tracking behavior trait of the EX2Level1
// class to always track serialized objects of that class.  This permits
// the system to detect and elminate redundent save/load operations.
// (It is concievable that this might someday be detected automatically
// but for now, this is not done so we have to rely on the programmer
// to specify this trait)
I3_CLASS_TRACKING(EX2Level1, track_always)

class EX2Level2_A : virtual public EX2Level1 {
public:
    template<class Archive>
    void save(Archive &ar, const unsigned int /* file_version */) const
    {
        std::cout << "Saving EX2Level2_A\n";
        ar << I3_SERIALIZATION_BASE_OBJECT_NVP(EX2Level1);
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int /* file_version */)
    {
        std::cout << "Restoring EX2Level2_A\n";
        ar >> I3_SERIALIZATION_BASE_OBJECT_NVP(EX2Level1);
    }

    I3_SERIALIZATION_SPLIT_MEMBER()
};

class EX2Level2_B : virtual public EX2Level1 {
public:    
    template<class Archive>
    void save(Archive &ar, const unsigned int /* file_version */) const
    {
        std::cout << "Saving EX2Level2_B\n";
        ar << I3_SERIALIZATION_BASE_OBJECT_NVP(EX2Level1);
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int /* file_version */)
    {
        std::cout << "Restoring EX2Level2_B\n";
        ar >> I3_SERIALIZATION_BASE_OBJECT_NVP(EX2Level1);
    }

    I3_SERIALIZATION_SPLIT_MEMBER()
};

class EX2Level3_A : public EX2Level2_A, public EX2Level2_B {
public:
    EX2Level3_A() {}
    EX2Level3_A(int i) : EX2Level1(i) {}

    template<class Archive>
    void save(Archive &ar, const unsigned int /* file_version */) const
    {
        std::cout << "Saving EX2Level3_A\n";
        ar << I3_SERIALIZATION_BASE_OBJECT_NVP(EX2Level2_A);
        ar << I3_SERIALIZATION_BASE_OBJECT_NVP(EX2Level2_B);
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int /* file_version */)
    {
        std::cout << "Restoring EX2Level3_A\n";
        ar >> I3_SERIALIZATION_BASE_OBJECT_NVP(EX2Level2_A);
        ar >> I3_SERIALIZATION_BASE_OBJECT_NVP(EX2Level2_B);
    }

    I3_SERIALIZATION_SPLIT_MEMBER()
};


class EX2Level3_B : public EX2Level2_A, public EX2Level2_B {
public:
    EX2Level3_B() {}
    EX2Level3_B(int i) : EX2Level1(i) {}

    template<class Archive>
    void save(Archive &ar, const unsigned int /* file_version */) const
    {
        std::cout << "Saving EX2Level3_B\n";
        ar << I3_SERIALIZATION_BASE_OBJECT_NVP(EX2Level2_A);
        ar << I3_SERIALIZATION_BASE_OBJECT_NVP(EX2Level2_B);
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int /* file_version */)
    {
        std::cout << "Restoring EX2Level3_B\n";
        ar >> I3_SERIALIZATION_BASE_OBJECT_NVP(EX2Level2_A);
        ar >> I3_SERIALIZATION_BASE_OBJECT_NVP(EX2Level2_B);
    }

    I3_SERIALIZATION_SPLIT_MEMBER()
};

class EX2Level4 : public EX2Level3_B {
public:
    EX2Level4() {}
    EX2Level4(int i) : EX2Level1(i) {}

    template<class Archive>
    void save(Archive &ar, const unsigned int /* file_version */) const
    {
        std::cout << "Saving EX2Level4\n";
        ar << I3_SERIALIZATION_BASE_OBJECT_NVP(EX2Level3_B);
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int /* file_version */)
    {
        std::cout << "Restoring EX2Level4\n";
        ar >> I3_SERIALIZATION_BASE_OBJECT_NVP(EX2Level3_B);
    }

    I3_SERIALIZATION_SPLIT_MEMBER()
};

I3_CLASS_EXPORT(EX1Level4)
I3_CLASS_EXPORT(EX1Level3_A)

I3_CLASS_EXPORT(EX2Level3_A)
I3_CLASS_EXPORT(EX2Level4)

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_diamond_complex");
    
    {
        save_count = 0;
        load_count = 0;

        const EX1Level3_A ex1L3a_save(3);
        const EX1Level1 *ex1L1_save = &ex1L3a_save;
        {
            typename TS::test_ostream ofs(testfile, TS::TEST_STREAM_FLAGS);
            typename TS::test_oarchive oa(ofs, TS::TEST_ARCHIVE_FLAGS);
            oa << icecube::serialization::make_nvp("ex1L1_save", ex1L1_save);
        }

        EX1Level1* ex1L1_load;
        {
            typename TS::test_istream ifs(testfile, TS::TEST_STREAM_FLAGS);
            typename TS::test_iarchive ia(ifs, TS::TEST_ARCHIVE_FLAGS);
            ia >> icecube::serialization::make_nvp("ex1L1_load", ex1L1_load);
        }
        ENSURE(1 == save_count);
        ENSURE(1 == load_count);
        ENSURE(*ex1L1_save == *ex1L1_load);
        std::remove(testfile.c_str());
    }
    {
        save_count = 0;
        load_count = 0;

        const EX1Level4 ex1L4_save(3);   
        const EX1Level1 *ex1L1_save = &ex1L4_save;
        {
            typename TS::test_ostream ofs(testfile, TS::TEST_STREAM_FLAGS);
            typename TS::test_oarchive oa(ofs, TS::TEST_ARCHIVE_FLAGS);
            oa << icecube::serialization::make_nvp("ex1L1_save", ex1L1_save);
        }

        EX1Level1* ex1L1_load;
        {
            typename TS::test_istream ifs(testfile, TS::TEST_STREAM_FLAGS);
            typename TS::test_iarchive ia(ifs, TS::TEST_ARCHIVE_FLAGS);
            ia >> icecube::serialization::make_nvp("ex1L1_load", ex1L1_load);
        }
        ENSURE(1 == save_count);
        ENSURE(1 == load_count);
        ENSURE(*ex1L1_save == *ex1L1_load);
        std::remove(testfile.c_str());
    }
    {
        save_count = 0;
        load_count = 0;

        const EX2Level3_A ex2L3a_save(3);
        const EX2Level1 *ex2L1_save = &ex2L3a_save;
        {
            typename TS::test_ostream ofs(testfile, TS::TEST_STREAM_FLAGS);
            typename TS::test_oarchive oa(ofs, TS::TEST_ARCHIVE_FLAGS);
            oa << icecube::serialization::make_nvp("ex2L1_save", ex2L1_save);
        }

        EX2Level1* ex2L1_load;
        {
            typename TS::test_istream ifs(testfile, TS::TEST_STREAM_FLAGS);
            typename TS::test_iarchive ia(ifs, TS::TEST_ARCHIVE_FLAGS);
            ia >> icecube::serialization::make_nvp("ex2L1_load", ex2L1_load);
        }
        ENSURE(1 == save_count);
        ENSURE(1 == load_count);
        ENSURE(*ex2L1_save == *ex2L1_load);
        std::remove(testfile.c_str());
    }
    {
        save_count = 0;
        load_count = 0;

        const EX2Level4 ex2L4_save(3);   
        const EX2Level1 *ex2L1_save = &ex2L4_save;
        {
            typename TS::test_ostream ofs(testfile, TS::TEST_STREAM_FLAGS);
            typename TS::test_oarchive oa(ofs, TS::TEST_ARCHIVE_FLAGS);
            oa << icecube::serialization::make_nvp("ex2L1_save", ex2L1_save);
        }

        EX2Level1* ex2L1_load;
        {
            typename TS::test_istream ifs(testfile, TS::TEST_STREAM_FLAGS);
            typename TS::test_iarchive ia(ifs, TS::TEST_ARCHIVE_FLAGS);
            ia >> icecube::serialization::make_nvp("ex2L1_load", ex2L1_load);
        }
        ENSURE(1 == save_count);
        ENSURE(1 == load_count);
        ENSURE(*ex2L1_save == *ex2L1_load);
        std::remove(testfile.c_str());
    }
}

TEST_GROUP(complex)

#define TEST_SET(name) \
TEST(name ## _test_diamond_complex){ \
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
