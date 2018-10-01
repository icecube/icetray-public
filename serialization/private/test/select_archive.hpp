/////////////////////////////////////////////
// invoke header for a custom archive test.

/////////////////////////////////////////////
// invoke header for a custom archive test.
#if ! defined(I3_ARCHIVE_TEST)
#define I3_ARCHIVE_TEST text_archive.hpp
#endif

#undef test_settings

#include <boost/preprocessor/stringize.hpp>
#include BOOST_PP_STRINGIZE(I3_ARCHIVE_TEST)

#ifndef TEST_DIRECTORY
#define TEST_DIRECTORY
#else
#define __x__ TEST_DIRECTORY
#undef TEST_DIRECTORY
#define TEST_DIRECTORY BOOST_PP_STRINGIZE(__x__)
#endif
