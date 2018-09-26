#include <icetray/serialization.h>
#include <dataclasses/geometry/I3Geometry.h>

#include <boost/foreach.hpp>

I3Geometry::~I3Geometry() {}

template <class Archive>
void 
I3Geometry::serialize(Archive& ar, unsigned version)
{
  if (version>i3geometry_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3Geometry class.", version, i3geometry_version_);

  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));

  if (version==0) {
    // these are now I3Maps and I3Vectors, not std::maps and std::vectors
    typedef std::map<OMKey, I3OMGeo> I3OMGeoMap_old;
    typedef std::map<int, std::vector<I3TankGeo> > I3StationGeoMap_old;
      
    I3OMGeoMap_old omgeo__;
    I3StationGeoMap_old stationgeo__;

    ar & make_nvp("OMGeo", omgeo__);
    ar & make_nvp("StationGeo", stationgeo__);

    omgeo.clear();
    BOOST_FOREACH(const I3OMGeoMap_old::value_type &pair, omgeo__) {
      omgeo.insert(pair);
    }

    stationgeo.clear();
    BOOST_FOREACH(const I3StationGeoMap_old::value_type &pair, stationgeo__) {
      stationgeo.insert(std::make_pair(pair.first,
                                       I3Vector<I3TankGeo>(pair.second.begin(), pair.second.end()) 
                                      ));
    }
  } else {
    ar & make_nvp("OMGeo", omgeo);
    ar & make_nvp("StationGeo", stationgeo);
  }

  ar & make_nvp("StartTime",startTime);
  ar & make_nvp("EndTime",endTime);
}

I3_SERIALIZABLE(I3Geometry);
