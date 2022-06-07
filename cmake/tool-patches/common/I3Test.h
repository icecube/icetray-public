/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *  
 *  SPDX-License-Identifier: BSD-2-Clause
 *  
 */
#ifndef I3TEST_H_INCLUDED
#define I3TEST_H_INCLUDED

#include <cmath>
#include <string>
#include <boost/version.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <map>
#include <set>
#include <cmath>

namespace fs = boost::filesystem;
namespace I3Test {

  typedef void (*voidfunc)();

  class I3TestException
  {
    std::string message;

  public:
    explicit I3TestException(const std::string& s) : message(s) { }
    virtual ~I3TestException() throw() { };
    virtual const char* what() const throw() { return message.c_str(); }
  };

  struct test_failure : I3TestException
  {
    unsigned line;
    std::string file, predicate, message;

    test_failure(const std::string& file_, unsigned line_, 
		 const std::string& predicate_, const std::string& message_ = "(none)"); 
  
    ~test_failure() throw() { } 
    std::string to_string(const std::string &name) const;
  };

  struct test_group 
  {
    std::map<std::string, voidfunc> units;
    std::map<std::string, boost::shared_ptr<test_failure> > failures;
    std::set<std::string> successes;

    void runtests(bool xml=false);
    void run(const std::string &unit, bool xml = false);
    void report(const std::string& group_name);
    static void first(const std::string &s, const std::pair<std::string, voidfunc>& p); 
    void list(const std::string& s);
  };

  struct test_suite 
  {
    std::map <std::string, test_group*>  groups;

    void runtests(bool xml=false);
    bool report();
    static void first(const std::pair<std::string, test_group*>& p);
    void list();
    void make_dartfiles(const std::string& path);
    void run(const std::string& s);

  };

#define ENSURE(PRED,...) \
  I3Test::ensure(__FILE__,__LINE__,PRED,BOOST_PP_STRINGIZE(PRED),##__VA_ARGS__)

#define FAIL(...) \
  I3Test::ensure(__FILE__,__LINE__,false,"FAIL",##__VA_ARGS__)

#define EXPECT_THROW(CMD, MSG)			    \
  try {                                             \
    CMD;                                            \
    FAIL(MSG);                \
   }catch (const std::exception &e) {/* good. */}
  
  
  inline 
  void ensure (const std::string& file, unsigned line, bool cond, const std::string& cond_txt,
	       const std::string& msg = "unspecified")
  {
    if (!cond)
      throw test_failure(file, line, cond_txt, msg);
  }

#define ENSURE_DISTANCE(LEFT,RIGHT,DISTANCE,...)			\
  I3Test::ensure_distance(__FILE__,__LINE__,				\
		  BOOST_PP_STRINGIZE(LEFT), BOOST_PP_STRINGIZE(RIGHT),	\
		  BOOST_PP_STRINGIZE(DISTANCE), LEFT,RIGHT,DISTANCE,	\
		  ##__VA_ARGS__);

  template <typename LeftType, typename RightType, typename ResultType>
  inline
  void ensure_distance (const std::string& file, unsigned line, 
			const std::string& left_txt, const std::string& right_txt, const std::string& distance_txt,
			const LeftType& actual, const RightType& expected, const ResultType& distance,
			const std::string& msg = "unspecified")

  {
    if (std::isnan(expected) || std::isnan(actual) || std::isnan(distance))
      {
	std::stringstream ss;
	ss << "ENSURE_DISTANCE(" << left_txt << ", " << right_txt << ", " << distance_txt
	   << "): " << left_txt << " == " << actual
	   << " " << right_txt << " == " << expected 
	   << " " << distance_txt << " == " << distance;
	throw test_failure(file, line, ss.str(), msg);
      }
    if( expected-distance >= actual || expected+distance <= actual )
      {
	std::stringstream ss;
	ss << "ensure_distance: expected [" << expected-distance << ";" 
	   << expected+distance << "] actual " << std::setprecision(16) << actual;
	throw test_failure(file, line, ss.str(), msg);
      }
  }

#define ENSURE_EQUAL(LEFT,RIGHT,...)					\
  I3Test::ensure_equal(__FILE__,__LINE__,				\
		       BOOST_PP_STRINGIZE(LEFT),			\
		       BOOST_PP_STRINGIZE(RIGHT),			\
		       LEFT,RIGHT,					\
		       ##__VA_ARGS__);

  template <typename LeftType, typename RightType>
  inline
  void ensure_equal (const std::string& file, unsigned line, 
		     const std::string& left_txt, const std::string& right_txt,
		     const LeftType& left, const RightType& right,
		     const std::string& msg = "unspecified")
  {
    if(!(left == right))
      {
	std::stringstream predstream;
	predstream << "ENSURE_EQUAL(" << left_txt << ", " << right_txt << "): "
		   << left_txt << " == " << left << ", " 
		  << right_txt << " == " << right;
	throw test_failure(file, line, predstream.str(), msg);
      }
  }

  extern test_suite& suite();

  struct test_registerer 
  {
    test_registerer(const std::string& group, const std::string& unit, voidfunc f);
  };

  struct group_registerer 
  {
    group_registerer(test_group* group, const std::string& name);
  };

#define TEST(TESTNAME)							\
  static void local_test_routine_ ## TESTNAME();			\
    namespace {								\
      static I3Test::test_registerer register_local_test_routine_ ## TESTNAME \
      (I3Test::group_name, BOOST_PP_STRINGIZE(TESTNAME), local_test_routine_ ## TESTNAME); \
    }									\
    static void local_test_routine_ ## TESTNAME() 
  
  /**
     Creates "temporary" file and a directory to place it in. Temporary in
     the sense that it is meant as a junk file for unit tests. It still
     needs to be deleted by hand.
     Returns the full path.
  */

  const std::string 
  testfile(const std::string& fname, const std::string& project = BOOST_PP_STRINGIZE(PROJECT));

} 

#if BOOST_VERSION > 104100
#define TEST_GROUP(GROUPNAME)						\
  namespace I3Test {							\
      static test_group *local_test_group() {				\
	static test_group group;					\
	return &group;							\
      }									\
      namespace {							\
	  static I3Test::group_registerer				\
	  registerer(local_test_group(), fs::path(__FILE__).filename().string());	\
	  static std::string group_name(fs::path(__FILE__).filename().string());		\
      }									\
  }
#else
#define TEST_GROUP(GROUPNAME)						\
  namespace I3Test {							\
      static test_group *local_test_group() {				\
	static test_group group;					\
	return &group;							\
      }									\
      namespace {							\
	  static I3Test::group_registerer				\
	  registerer(local_test_group(), fs::path(__FILE__).leaf());	\
	  static std::string group_name(fs::path(__FILE__).leaf());		\
      }									\
  }
#endif

#endif
