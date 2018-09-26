#include <I3Test.h>
#include "phys-services/geo-selector/I3GeometrySelectorServiceFactory.h"
#include "phys-services/geo-selector/I3GeoSelTestModule.h"
#include "phys-services/geo-selector/GeoSelUtils.h"
#include "icetray/I3Tray.h"
#include "phys-services/source/I3TextFileGeometryServiceFactory.h"
#include "icetray/I3Units.h"

using geo_sel_utils::make_good_strings;
using geo_sel_utils::exists;
using geo_sel_utils::exists_at;
using geo_sel_utils::parse_string_list;
using geo_sel_utils::good_input;

class SetDrivingTime : public I3Module {
public:
	SetDrivingTime(const I3Context &ctx) : I3Module(ctx) {
		AddOutBox("OutBox");
	}
	void Physics(I3FramePtr fr) {
		I3TimePtr time(new I3Time(2006, 0));
		fr->Put("DrivingTime", time);
		PushFrame(fr);
	}
};

I3_MODULE(SetDrivingTime);


TEST_GROUP(GeometrySelector);

TEST(exists_function){

  int seed = time(0);
  std::vector<int> vec;
  {
    srand(seed);
    int n_to_find = rand();
    int n_to_load = rand()%10000;
    int position = rand()%n_to_load;
    bool loaded = false;
    for(int i=0; i<n_to_load; i++){
      if(i==position){
	vec.push_back(n_to_find);
	loaded = true;
      }else{
	vec.push_back(rand());
      }
    }
    ENSURE(loaded,"you won't find the number you're looking for");
  }
  {
    srand(seed);
    int n = rand();
    bool found = exists<int>(n,vec);
    ENSURE(found,"didn't find the number.  exists not working.");
  }
}

TEST(parse_string_list){

  std::string string_list("1,2,3,4,9:11,-8");
  std::vector<int> parsed_list = parse_string_list(string_list);
  ENSURE(parsed_list.size() == 8);

}

TEST(make_good_strings_simple){

  std::vector<int> good_strings;
  const std::string to_use("1,2,3,4");
  const std::string to_exclude("");

  good_strings = make_good_strings(to_use,to_exclude);
  ENSURE(exists<int>(1,good_strings));
  ENSURE(exists<int>(2,good_strings));
  ENSURE(exists<int>(3,good_strings));
  ENSURE(exists<int>(4,good_strings));
  ENSURE(good_strings.size() == 4);
}

TEST(make_good_strings_more_complex){

  std::vector<int> good_strings;
  const std::string to_use("1,2,3,4,84,9:15");
  const std::string to_exclude("");

  good_strings = make_good_strings(to_use,to_exclude);
  ENSURE(exists<int>(1,good_strings));
  ENSURE(exists<int>(2,good_strings));
  ENSURE(exists<int>(3,good_strings));
  ENSURE(exists<int>(4,good_strings));

  ENSURE(exists<int>(84,good_strings));

  ENSURE(exists<int>(9,good_strings));
  ENSURE(exists<int>(10,good_strings));
  ENSURE(exists<int>(11,good_strings));
  ENSURE(exists<int>(12,good_strings));
  ENSURE(exists<int>(13,good_strings));
  ENSURE(exists<int>(14,good_strings));
  ENSURE(exists<int>(15,good_strings));

  ENSURE(good_strings.size() == 12);
}

TEST(make_good_strings_with_excludes){

  std::vector<int> good_strings;
  const std::string to_use("1,2,3,4,84,9:15,-10");
  const std::string to_exclude("3,11:13,99");

  good_strings = make_good_strings(to_use,to_exclude);
  ENSURE(exists<int>(1,good_strings));
  ENSURE(exists<int>(2,good_strings));
  ENSURE(!exists<int>(3,good_strings));
  ENSURE(exists<int>(4,good_strings));

  ENSURE(exists<int>(84,good_strings));

  ENSURE(exists<int>(9,good_strings));
  ENSURE(exists<int>(10,good_strings));
  ENSURE(!exists<int>(11,good_strings));
  ENSURE(!exists<int>(12,good_strings));
  ENSURE(!exists<int>(13,good_strings));
  ENSURE(exists<int>(14,good_strings));
  ENSURE(exists<int>(15,good_strings));

  ENSURE(exists<int>(-10,good_strings));

  ENSURE(!exists<int>(99,good_strings));

  ENSURE(good_strings.size() == 9);
}

TEST(good_input){
  std::vector<int> good_strings;
  const std::string to_use_good("1,2,3,4,84,9:15");
  const std::string to_exclude_good("3,11:13,99");

  const std::string to_use_bad("1,2,3,4,#84,9:15");
  const std::string to_exclude_bad("3,11:13,?99");

  ENSURE(good_input(to_use_good));
  ENSURE(good_input(to_exclude_good));

  ENSURE(!good_input(to_use_bad));
  ENSURE(!good_input(to_exclude_bad));

}

TEST(icetray_test){

  I3Tray tray;

  std::string strings_to_use("21,29,39,38,30,40,49");
  std::string stations_to_use("21,29,30,38,39,40,47,48,49,50,57,58,59,66,67,74");

  std::string icecube_geo(getenv("I3_SRC"));
  icecube_geo += "/phys-services/resources/icecube.geo";
  std::string amanda_geo(getenv("I3_SRC"));
  amanda_geo += "/phys-services/resources/amanda.geo";

  tray.AddService("I3TextFileGeometryServiceFactory","geoservice")
    ("IceCubeGeoFile",icecube_geo.c_str())
    ("AmandaGeoFile",amanda_geo.c_str());
  tray.AddService("I3GeometrySelectorServiceFactory","geo_selector")
    ("StringsToUse",strings_to_use.c_str())
    ("StationsToUse",stations_to_use.c_str())
    ("GeoSelectorName","I3GeometrySelectorService");
  tray.AddModule("I3InfiniteSource", "source")("Stream",I3Frame::Physics);
  tray.AddModule("SetDrivingTime", "time");
  tray.AddModule("I3MetaSynth","muxer")
    ("GeometryService","I3GeometrySelectorService");
  //I3GeoSelTestModule contains ENSURE statements
  tray.AddModule("I3GeoSelTestModule","geo_test") 
    ("StringsToUse",strings_to_use.c_str())
    ("StationsToUse",stations_to_use.c_str());

  tray.Execute(4);
}

TEST(icetray_test_shift){

  I3Tray tray;

  std::string strings_to_use("21,29,39,38,30,40,49");
  std::string stations_to_use("21,29,30,38,39,40,47,48,49,50,57,58,59,66,67,74");

  std::string strings_to_exclude("-1:20,22:28,31:37,41:48,50:80");
  std::string stations_to_exclude("-1:20,22:28,31:37,41:46,51:56,60:65,68:73,75:80");

  std::string icecube_geo(getenv("I3_SRC"));
  icecube_geo += "/phys-services/resources/icecube.geo";
  std::string amanda_geo(getenv("I3_SRC"));
  amanda_geo += "/phys-services/resources/amanda.geo";

  tray.AddService("I3TextFileGeometryServiceFactory","geoservice")
    ("IceCubeGeoFile",icecube_geo.c_str())
    ("AmandaGeoFile",amanda_geo.c_str());
  tray.AddService("I3GeometrySelectorServiceFactory","geo_selector")
    ("StringsToUse",strings_to_use.c_str())
    ("StationsToUse",stations_to_use.c_str())
    ("GeoSelectorName","I3GeometrySelectorService")
    ("ShiftX",100*I3Units::m)
    ("ShiftY",100*I3Units::m)
    ("ShiftZ",100*I3Units::m);

  tray.AddModule("I3InfiniteSource", "source")("Stream",I3Frame::Physics);
  tray.AddModule("SetDrivingTime", "time");
  tray.AddModule("I3MetaSynth","muxer")
    ("GeometryService","I3GeometrySelectorService");

  //I3GeoSelTestModule contains ENSURE statements
  tray.AddModule("I3GeoSelTestModule","geo_test") 
    ("StringsToUse",strings_to_use.c_str())
    ("StringsToExclude",strings_to_exclude.c_str())
    ("StationsToUse",stations_to_use.c_str())
    ("StationsToExclude",stations_to_exclude.c_str())
    ("ShiftX",100*I3Units::m)
    ("ShiftY",100*I3Units::m)
    ("ShiftZ",100*I3Units::m);

  tray.Execute(4);
}

TEST(detector_shift){

  I3Geometry geo;

  OMKey om1(21,1);
  I3Position p1(15.,5.,0.);
  I3OMGeo g1;
  g1.position = p1;

  OMKey om2(29,4);
  I3Position p2(-5.,-15.,0.);
  I3OMGeo g2;
  g2.position = p2;

  OMKey om3(19,8);
  I3Position p3(105.,50.,11.);
  I3OMGeo g3;
  g3.position = p3;

  OMKey om4(32,20);
  I3Position p4(-333.,-18.,400.);
  I3OMGeo g4;
  g4.position = p4;

  geo.omgeo[om1] = g1; 
  geo.omgeo[om2] = g2; 
  geo.omgeo[om3] = g3; 
  geo.omgeo[om4] = g4; 

  std::vector<int> goodStrings;
  goodStrings.push_back(21);
  goodStrings.push_back(29);

  I3GeometryConstPtr geo_cptr(new I3Geometry(geo));
  std::pair<double,double> c =
    geo_sel_utils::detector_center(geo_cptr,goodStrings);

  ENSURE(c.first==5.);
  ENSURE(c.second==-5.);
}

TEST(icetray_test_center_shift){

  I3Tray tray;

  std::string strings_to_use("21,29,39,38,30,40,49");
  std::string stations_to_use("21,29,30,38,39,40,47,48,49,50,57,58,59,66,67,74");

  std::string strings_to_exclude("-1:20,22:28,31:37,41:48,50:80");
  std::string stations_to_exclude("-1:20,22:28,31:37,41:46,51:56,60:65,68:73,75:80");

  std::string icecube_geo(getenv("I3_SRC"));
  icecube_geo += "/phys-services/resources/icecube.geo";
  std::string amanda_geo(getenv("I3_SRC"));
  amanda_geo += "/phys-services/resources/amanda.geo";

  tray.AddService("I3TextFileGeometryServiceFactory","geoservice")
    ("IceCubeGeoFile",icecube_geo.c_str())
    ("AmandaGeoFile",amanda_geo.c_str());
  tray.AddService("I3GeometrySelectorServiceFactory","geo_selector")
    ("StringsToUse",strings_to_use.c_str())
    ("StationsToUse",stations_to_use.c_str())
    ("GeoSelectorName","I3GeometrySelectorService")
    ("ShiftToCenter",true);

  tray.AddModule("I3InfiniteSource", "source")("Stream",I3Frame::Physics);
  tray.AddModule("SetDrivingTime", "time");
  tray.AddModule("I3MetaSynth","muxer")
    ("GeometryService","I3GeometrySelectorService");
  //I3GeoSelTestModule contains ENSURE statements
  tray.AddModule("I3GeoSelTestModule","geo_test") 
    ("StringsToUse",strings_to_use.c_str())
    ("StringsToExclude",strings_to_exclude.c_str())
    ("StationsToUse",stations_to_use.c_str())
    ("StationsToExclude",stations_to_exclude.c_str())
    ("DetectorCentered",true);

  tray.Execute(4);
}
