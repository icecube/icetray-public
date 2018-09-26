#include "phys-services/source/I3TextFileGeometryService.h"
#include "icetray/I3Units.h"
#include "dataclasses/geometry/I3OMGeo.h"
#include "icetray/I3TrayHeaders.h"
#include <dataclasses/I3Time.h>
#include <dataclasses/geometry/I3Geometry.h>

void I3TextFileGeometryService::Fatal(const std::string& message)
{
  log_fatal("%s", message.c_str());
}

I3GeometryConstPtr I3TextFileGeometryService::GetGeometry(I3Time time)
{
  I3GeometryPtr geometry = I3GeometryPtr(new I3Geometry());
  FillGeometryFromFile(*geometry);
  I3Time start;
  start.SetDaqTime(0,0);
  I3Time end;
  end.SetDaqTime(3000,0);
  geometry->startTime = start;
  geometry->endTime = end;
  return geometry;
}

void I3TextFileGeometryService::FillGeometryFromFile(I3Geometry& Geometry)
{
  //
  // NB: This code was cut and pasted with minor tweaks from the I3Db code.
  // The author is Georges Kohnen
  //
  // Any bugs are probably my fault, and any praises should
  // probably go to him. -JPretz
  //
  
  //Create GeoInFile
  std::ifstream AmaGeoInFile;	
  std::ifstream I3GeoInFile;		
  log_warn("Reading IceCube Geometry from file");
  
  //Open the files ama.geo and icecube.geo
  AmaGeoInFile.open(fAmaGeoInputFile.c_str(), std::ifstream::in); 	
  I3GeoInFile.open(fI3GeoInputFile.c_str(), std::ifstream::in); 
  
  //Did the files open correctly?
  if (AmaGeoInFile.fail())
    log_fatal("The specified AMANDA geometry file does not exist "
	  "or did not want to be opened!");
  if (I3GeoInFile.fail())
    log_fatal("The specified IceCube geometry file does not exist "
	  "or did not want to be opened!");	
  
  log_info("FILES opened successfully. Getting the geometry "
	   "and filling it into the frame");
  log_info("Coordinates of OM's in meters \n\n");
  
  //The variables declared hereafter are followed by "_F" to denote 
  //that they serve the
  //"Fill Geometry From File" method and not the DB Access method...
  
  int string_F;
  int tube_F;
  int orientation_F;		
  double x_F, y_F, z_F;

  // Changed all objects that were I3InIceGeometry (the vars amanda and icecube)  to the I3Geometry's omgeo map.
  
  while(AmaGeoInFile>>string_F>>tube_F>>x_F>>y_F>>z_F>>orientation_F)
    {    
      I3OMGeo amanda;
      amanda.position=I3Position(x_F * I3Units::m,
				  y_F * I3Units::m,
				  z_F * I3Units::m,
				  I3Position::car);
      if (orientation_F == -1) amanda.orientation = I3Orientation(0.,0.,-1., 1.,0.,0.); // down
      else if (orientation_F == 1) amanda.orientation = I3Orientation(0.,0.,1., 1.,0.,0.); // up
      amanda.area = (0.0284 * I3Units::m2);
      amanda.omtype = I3OMGeo::AMANDA;
      Geometry.omgeo[OMKey(string_F, tube_F)] = amanda;
    }
  
  while(I3GeoInFile>>string_F>>tube_F>>x_F>>y_F>>z_F>>orientation_F)
    {
      if(tube_F<61)  //InIce
      {
        I3OMGeo icecube;
        icecube.position=I3Position(x_F * I3Units::m,
				     y_F * I3Units::m,
				     z_F * I3Units::m,
				     I3Position::car);
        icecube.orientation = I3Orientation(0.,0.,-1., 1.,0.,0.); // down 
        icecube.area = (0.0444 * I3Units::m2);
	icecube.omtype = I3OMGeo::IceCube;
	Geometry.omgeo[OMKey(string_F, tube_F)] = icecube;
      }
      else //IceTop
      {
	I3StationGeoMap &station_geo = Geometry.stationgeo;
	//Initializes a new I3StationGeo Object if it does not already exist.
	//Condensed initialization into one step (originally created new vector
	//and added pushed two new TankGeo objects into it.
        if(station_geo.find(string_F)==station_geo.end()) 
        {
          station_geo[string_F] = I3StationGeo(2);
        }

        I3OMGeo icecube;
        icecube.position=I3Position(x_F * I3Units::m,
				      y_F * I3Units::m,
				      z_F * I3Units::m,
				      I3Position::car);
        icecube.orientation = I3Orientation(0.,0.,-1., 1.,0.,0.); // down 
        icecube.area = (0.0444 * I3Units::m2);
	icecube.omtype = I3OMGeo::IceTop;
	Geometry.omgeo[OMKey(string_F, tube_F)] = icecube;
      }
    }

  // average the dom positions to calculate the tank positions
  for (I3StationGeoMap::iterator
       iter_station = Geometry.stationgeo.begin();
       iter_station != Geometry.stationgeo.end();
       ++iter_station) {
       
    int station_id = iter_station->first;
    
    //Set the tank positions
    I3Position p61 (Geometry.omgeo[OMKey (station_id, 61)].position);
    I3Position p62 (Geometry.omgeo[OMKey (station_id, 62)].position);
    I3Position p63 (Geometry.omgeo[OMKey (station_id, 63)].position);
    I3Position p64 (Geometry.omgeo[OMKey (station_id, 64)].position);
    
    //DOMs 61 and 62 -> Tank A
    iter_station->second[0].position = I3Position(.5 * (p61.GetX () + p62.GetX ()),
						  .5 * (p61.GetY () + p62.GetY ()),
						  .5 * (p61.GetZ () + p62.GetZ ()));
    
    iter_station->second[0].omKeyList_.push_back(OMKey(iter_station->first, 61));
    iter_station->second[0].omKeyList_.push_back(OMKey(iter_station->first, 62));
    
    //DOMs 63 and 64 -> Tank B
    iter_station->second[1].position = I3Position(.5 * (p63.GetX () + p64.GetX ()),
						  .5 * (p63.GetY () + p64.GetY ()),
						  .5 * (p63.GetZ () + p64.GetZ ()));
     
    iter_station->second[1].omKeyList_.push_back (OMKey (iter_station->first, 63));
    iter_station->second[1].omKeyList_.push_back (OMKey (iter_station->first, 64));
     
    //Set const values
    const double tankradius = 0.93 * I3Units::m;
    const double tankheight = 1.30 * I3Units::m;
    const double fillheight = 0.90 * I3Units::m;
    iter_station->second[0].tankradius = tankradius;
    iter_station->second[0].tankheight = tankheight;
    iter_station->second[0].fillheight = fillheight;
    iter_station->second[1].tankradius = tankradius;
    iter_station->second[1].tankheight = tankheight;
    iter_station->second[1].fillheight = fillheight;
     
    if (station_id == 21 || station_id == 29 || station_id == 30 || station_id == 39 ) {
      iter_station->second[0].tanktype = I3TankGeo::Tyvek_Lined;
      iter_station->second[1].tanktype = I3TankGeo::Tyvek_Lined;
    }
    else {
      iter_station->second[0].tanktype = I3TankGeo::Zirconium_Lined;
      iter_station->second[1].tanktype = I3TankGeo::Zirconium_Lined;
    }
     
    //Set SnowHeights
    iter_station->second[0].snowheight = 0;
    iter_station->second[1].snowheight = 0;
  }
  
  AmaGeoInFile.close();
  I3GeoInFile.close();
  
  //Integrate into DB Access part too?
  log_warn("FillFileGeometry: ATTENTION: No date set in header or date set to 0.");
  
  I3Time time;
  time.SetModJulianTime(0,0,0.0);
  Geometry.startTime = time;
}

