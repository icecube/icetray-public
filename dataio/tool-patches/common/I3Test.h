/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *  
 */
#ifndef I3TEST_H_INCLUDED
#define I3TEST_H_INCLUDED

#include <boost/preprocessor/stringize.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <map>
#include <set>

namespace fs = boost::filesystem;
namespace I3Test {

  using namespace std;

  typedef void (*voidfunc)();

  class I3TestException
  {
    string message;

  public:
    explicit I3TestException(const string& s) : message(s) { }
    virtual ~I3TestException() throw() { };
    virtual const char* what() const throw() { return message.c_str(); }
  };

  struct test_failure : I3TestException
  {
    unsigned line;
    string file, predicate, message;

    test_failure(const string& file_, unsigned line_, 
		 const string& predicate_, const string& message_ = "(none)"); 
  
    ~test_failure() throw() { } 
    string to_string(const string &name) const;
  };

  struct test_group 
  {
    map<string, voidfunc> units;
    map<string, boost::shared_ptr<test_failure> > failures;
    set<string> successes;

    void runtests(bool xml=false);
    void run(const string &unit, bool xml = false);
    void report(const string& group_name);
    static void first(const string &s, const pair<string, voidfunc>& p); 
    void list(const string& s);
  };

  struct test_suite 
  {
    map <string, test_group*>  groups;

    void runtests(bool xml=false);
    bool report();
    static void first(const pair<string, test_group*>& p);
    void list();
    void make_dartfiles(const string& path);
    void run(const string& s);

  };

#define ENSURE(PRED,...) \
  I3Test::ensure(__FILE__,__LINE__,PRED,BOOST_PP_STRINGIZE(PRED),##__VA_ARGS__)

#define FAIL(...) \
  I3Test::ensure(__FILE__,__LINE__,false,"FAIL",##__VA_ARGS__)

  inline 
  void ensure (const string& file, unsigned line, bool cond, const string& cond_txt,
	       const string& msg = "unspecified")
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
  void ensure_distance (const string& file, unsigned line, 
			const string& left_txt, const string& right_txt, const string& distance_txt,
			const LeftType& actual, const RightType& expected, const ResultType& distance,
			const string& msg = "unspecified")

  {
    if (isnan(expected) || isnan(actual) || isnan(distance))
      {
	stringstream ss;
	ss << "ENSURE_DISTANCE(" << left_txt << ", " << right_txt << ", " << distance_txt
	   << "): " << left_txt << " == " << actual
	   << " " << right_txt << " == " << expected 
	   << " " << distance_txt << " == " << distance;
	throw test_failure(file, line, ss.str(), msg);
      }
    if( expected-distance >= actual || expected+distance <= actual )
      {
	stringstream ss;
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
  void ensure_equal (const string& file, unsigned line, 
		     const string& left_txt, const string& right_txt,
		     const LeftType& left, const RightType& right,
		     const string& msg = "unspecified")
  {
    if(!(left == right))
      {
	stringstream predstream;
	predstream << "ENSURE_EQUAL(" << left_txt << ", " << right_txt << "): "
		   << left_txt << " == " << left << ", " 
		  << right_txt << " == " << right;
	throw test_failure(file, line, predstream.str(), msg);
      }
  }

  extern test_suite& suite();

  struct test_registerer 
  {
    test_registerer(const string& group, const string& unit, voidfunc f);
  };

  struct group_registerer 
  {
    group_registerer(test_group* group, const string& name);
  };

#define TEST(TESTNAME)							\
  static void local_test_routine_ ## TESTNAME();			\
    namespace {								\
      static I3Test::test_registerer register_local_test_routine_ ## TESTNAME \
      (I3Test::group_name, BOOST_PP_STRINGIZE(TESTNAME), local_test_routine_ ## TESTNAME); \
    }									\
    static void local_test_routine_ ## TESTNAME() 
  
  /**
     Creates the correct directory for temporary files to go in,
     returns the full path. 
  */

  std::string 
  tempfile(const std::string& fname, const std::string& project = BOOST_PP_STRINGIZE(PROJECT));

} 

#define TEST_GROUP(GROUPNAME)						\
  namespace I3Test {							\
      static test_group *local_test_group() {				\
	static test_group group;					\
	return &group;							\
      }									\
      namespace {							\
	  static I3Test::group_registerer				\
	  registerer(local_test_group(), fs::path(__FILE__).leaf());	\
	  static string group_name(fs::path(__FILE__).leaf());		\
      }									\
  }

#endif
