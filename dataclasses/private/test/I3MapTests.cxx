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
#include "icetray/open.h"
#include "icetray/I3Frame.h"
#include <boost/iostreams/filtering_stream.hpp>

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


TEST(GetTankGeoX_fromOMKey)
{
  // Let's look at some real GCD's with known behavior, and grab their I3Geometry, and access the I3TankGeo's

  // This one should have "conventional ordering" for some stations, but "swapped ordering" for others
  std::string td(getenv("I3_TESTDATA"));
  std::string filename = td+"/GCD/GeoCalibDetectorStatus_2020.Run134142_V0.i3.gz";

  // Get the Geometry from the G frame
  // (I stole/modified this bit from finiteReco/private/test/I3FiniteCalcTest.cxx --nice!)
  std::vector<std::string> skip;
  boost::iostreams::filtering_istream ifs;
  I3::dataio::open(ifs, filename);
  I3Frame frame;
  I3Frame::Stream s;;
  do{
    frame.load(ifs, skip);
    s = frame.GetStop();
    ENSURE(s!=I3Frame::None);
  }while(s!=I3Frame::Geometry);  
  I3GeometryConstPtr geometry1 = frame.Get<I3GeometryConstPtr>();

  // The real X coordinates
  double realx_2A = -140.360;
  double realx_2B = -130.635;
  double realx_3A = -27.720;
  double realx_3B = -20.395;
  
  //Now. lets' try to access....using GetTankGeo in various ways
  // Station 2: conventional ordering
  I3TankGeo tg_2_61 = geometry1->GetTankGeo(OMKey(2,61));
  I3TankGeo tg_2_A = geometry1->GetTankGeo(TankKey(2,TankKey::TankA));
  ENSURE_DISTANCE(tg_2_61.position.GetX(),realx_2A, 0.0001, "Bad X value from GetTankGeo(2,61)");
  ENSURE_DISTANCE(tg_2_A.position.GetX(),realx_2A, 0.0001, "Bad X value from GetTankGeo(2,A)");
  I3TankGeo tg_2_64 = geometry1->GetTankGeo(OMKey(2,64));
  I3TankGeo tg_2_B = geometry1->GetTankGeo(TankKey(2,TankKey::TankB));
  ENSURE_DISTANCE(tg_2_64.position.GetX(),realx_2B, 0.0001, "Bad X value from GetTankGeo(2,64)");
  ENSURE_DISTANCE(tg_2_B.position.GetX(),realx_2B, 0.0001, "Bad X value from GetTankGeo(2,B)");
  // Station 3: swapped ordering -- should still work, regardless!
  I3TankGeo tg_3_61 = geometry1->GetTankGeo(OMKey(3,61));
  I3TankGeo tg_3_A = geometry1->GetTankGeo(TankKey(3,TankKey::TankA));
  ENSURE_DISTANCE(tg_3_61.position.GetX(),realx_3A, 0.0001, "Bad X value from GetTankGeo(3,61)");
  ENSURE_DISTANCE(tg_3_A.position.GetX(),realx_3A, 0.0001, "Bad X value from GetTankGeo(3,A)");
  I3TankGeo tg_3_64 = geometry1->GetTankGeo(OMKey(3,64));
  I3TankGeo tg_3_B = geometry1->GetTankGeo(TankKey(3,TankKey::TankB));
  ENSURE_DISTANCE(tg_3_64.position.GetX(),realx_3B, 0.0001, "Bad X value from GetTankGeo(3,64)");
  ENSURE_DISTANCE(tg_3_B.position.GetX(),realx_3B, 0.0001, "Bad X value from GetTankGeo(3,B)");

  //Look for something not there.
  try{
    I3TankGeo badtry = geometry1->GetTankGeo(OMKey(85,61));  // No Station 85.
    //I3MapGet(fives,georef.omgeo).position.GetX();
    FAIL("That Get() call should not work");
  } catch (const std::exception&e) { }

  //Look for something else not there.
  try{
    I3TankGeo badtry = geometry1->GetTankGeo(OMKey(20,5));  // DOM 5 is not IceTop
    //I3MapGet(fives,georef.omgeo).position.GetX();
    FAIL("That Get() call should not work");
  } catch (const std::exception&e) { }

}


