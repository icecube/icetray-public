/**
 * $Id$
 */

#include <sstream>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/python.hpp>

#include <I3Test.h>
#include <icetray/I3Parameter.h>

using namespace std;

TEST_GROUP(I3Parameter);

#define THROW(sth) try { sth; FAIL("that should have thrown"); } catch (...) { PyErr_Clear(); }

TEST(noconvert)
{
  I3Parameter pi;
  pi.name("pi");
  ENSURE(! pi.has_default());
  ENSURE(! pi.has_configured());

  THROW(pi.value());

  pi.set_default(boost::python::object(1));
  //  THROW(pi.get());

  ENSURE_EQUAL(pi.get<int>(), 1);
  pi.set_default(boost::python::object(2));
  ENSURE_EQUAL(pi.get<int>(), 2);
  pi.set_default(boost::python::object(3.14));
  THROW(pi.get<int>());
}

TEST(somevectors)
{
  I3Parameter pi;
  pi.name("pi");

  vector<unsigned> vu;
  I3Parameter pv;
  pv.set_default(boost::python::object(vu));

  vector<short> vs;
  vs.push_back(1);
  vs.push_back(-2);

  pv.set_configured(boost::python::object(vs));  // should be ok.. but then
  // throws on conversion of vector element
  THROW(pv.get<vector<unsigned> >());

}

