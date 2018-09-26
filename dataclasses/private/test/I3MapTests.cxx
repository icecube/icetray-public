/**
    copyright  (C) 2004
    the icecube collaboration
    $Id$

    @version $Revision$
    @author blaufuss

*/

#include <I3Test.h>

#include "dataclasses/geometry/I3Geometry.h"
#include "dataclasses/Utility.h"

TEST_GROUP(I3MapStuff);

/**
 * Check getting an item out.
 */

TEST(GetOMGeoX)
{
  I3Geometry geo;
  const I3Geometry& georef = geo;
  I3OMGeo omone;
  I3OMGeo omtwo;
  I3Position posone(100.0,200.0,300.0);
  I3Position postwo(110.0,210.0,310.0);
  OMKey keyone(21,22);
  OMKey keytwo(31,32);


  omone.position = posone;
  omtwo.position = postwo;

  // Const cast to fake getting a const object from frame.
  const_cast<I3Geometry&>(georef).omgeo[keyone] = omone;
  const_cast<I3Geometry&>(georef).omgeo[keytwo] = omtwo;

  //Now. lets' try to access....using Get
  
  double hitx = I3MapGet(keyone,georef.omgeo).position.GetX(); 
  ENSURE_DISTANCE(hitx,posone.GetX(),0.01,"Bad X value from std::map");

  //Look for something not there.
  OMKey fives(5,5);
  try{
    I3MapGet(fives,georef.omgeo).position.GetX();
    FAIL("That Get() call should not work");
  } catch (const std::exception&e) { }

}


TEST(stlmapwithI3Get)
{
  std::map<std::string,int> m;
  m["hello"] = 5;
  std::cout<<I3MapGet("hello",m)<<std::endl;
  ENSURE_EQUAL(I3MapGet("hello",m),5,"non-const std::map");

  const std::map<std::string,int>& cm = m;
  std::cout<<I3MapGet("hello",cm)<<std::endl;
  ENSURE_EQUAL(I3MapGet("hello",cm),5,"non-const std::map");

  try 
    {
      std::cout<<I3MapGet("foo",m)<<std::endl;
      ENSURE(0,"Should have thrown");
    }
  catch(const std::exception& e)
    {

    }

  try 
    {
      std::cout<<I3MapGet("foo",cm)<<std::endl;
      ENSURE(0,"Should have thrown");
    }
  catch(const std::exception& e)
    {

    }

}


TEST(I3MapwithI3Get)
{
  I3Map<std::string,int> m;
  m["hello"] = 5;
  std::cout<<I3MapGet("hello",m)<<std::endl;
  ENSURE_EQUAL(I3MapGet("hello",m),5,"non-const std::map");

  const I3Map<std::string,int>& cm = m;
  std::cout<<I3MapGet("hello",cm)<<std::endl;
  ENSURE_EQUAL(I3MapGet("hello",cm),5,"non-const std::map");

  try 
    {
      std::cout<<I3MapGet("foo",m)<<std::endl;
      ENSURE(0,"Should have thrown");
    }
  catch(const std::exception& e)
    {

    }

  try 
    {
      std::cout<<I3MapGet("foo",cm)<<std::endl;
      ENSURE(0,"Should have thrown");
    }
  catch(const std::exception& e)
    {

    }

}


TEST(I3MapwithI3GetandOMKeys)
{
  I3Map<OMKey,int> m;
  m[OMKey(4,5)] = 5;
  std::cout<<I3MapGet(OMKey(4,5),m)<<std::endl;
  ENSURE_EQUAL(I3MapGet(OMKey(4,5),m),5,"non-const std::map");

  const I3Map<OMKey,int>& cm = m;
  std::cout<<I3MapGet(OMKey(4,5),cm)<<std::endl;
  ENSURE_EQUAL(I3MapGet(OMKey(4,5),cm),5,"non-const std::map");

  try 
    {
      std::cout<<I3MapGet(OMKey(4,4),m)<<std::endl;
      ENSURE(0,"Should have thrown");
    }
  catch(const std::exception& e)
    {

    }

  try 
    {
      std::cout<<I3MapGet(OMKey(4,4),cm)<<std::endl;
      ENSURE(0,"Should have thrown");
    }
  catch(const std::exception& e)
    {

    }

}
