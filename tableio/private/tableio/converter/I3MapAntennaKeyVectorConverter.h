// SPDX-FileCopyrightText: 2025 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

// A converter can be made for frame objects of the form map<AntennaKey, vector<T>> by
// specializing the following templated struct for T. 

// based on I3MapConverter.h

#ifndef TABLEIO_I3MAPANTENNAKEYVECTORCONVERTER_H_INCLUDED
#define TABLEIO_I3MAPANTENNAKEYVECTORCONVERTER_H_INCLUDED

#include <tableio/I3Converter.h>
#include <dataclasses/I3Map.h>
#include <dataclasses/geometry/I3Geometry.h>
#include <icetray/I3Frame.h>
#include <tableio/converter/container_converter_detail.h>
#include <dataclasses/AntennaKey.h>


template <class converter_type,
          typename map_type = I3Map<AntennaKey, std::vector<typename converter_type::booked_type> >,
          typename frameobject_type = map_type >
class I3MapAntennaKeyVectorConverter 
  : public I3ConverterImplementation< frameobject_type > 
{
public:
  I3MapAntennaKeyVectorConverter(bool bookGeometry)
    : bookGeometry_(bookGeometry) 
  {}

  I3MapAntennaKeyVectorConverter()
    : bookGeometry_(false)
  {}

    
protected:
  size_t GetNumberOfRows(const map_type& m) {
    log_trace("%s", __PRETTY_FUNCTION__);
    typename map_type::const_iterator iter = m.begin();
    size_t nrows = 0;
    while (iter != m.end())
      {
        nrows += iter->second.size();
        ++iter;
      }
    return nrows;
  }

  I3TableRowDescriptionPtr CreateDescription(const map_type& m) 
  {
    log_trace("%s", __PRETTY_FUNCTION__);
    I3TableRowDescriptionPtr desc = 
      I3TableRowDescriptionPtr(new I3TableRowDescription() );
    desc->isMultiRow_ = true;
    desc->AddField<int32_t>("station", "", "Station number");
    desc->AddField<uint32_t>("antenna", "", "antenna number");
    if (bookGeometry_) {
      desc->AddField<double>("x", "m", "X coordinate of the antenna");
      desc->AddField<double>("y", "m", "Y coordinate of the antenna");
      desc->AddField<double>("z", "m", "Z coordinate of the antenna");
    }
    desc->AddField<tableio_size_t>("vector_index", "", "index in vector");
      
    if (m.size() && m.begin()->second.size()) {
      detail::add_fields(converter_, desc, *(m.begin()->second.begin()));
    } else {
      typedef typename map_type::mapped_type::value_type value_type;
      detail::add_fields(converter_, desc, value_type());
    }

    return desc;
  }

  size_t FillRows(const map_type& m, I3TableRowPtr rows) 
  {
    static int nGeometryWarnings = 0;

    log_trace("%s", __PRETTY_FUNCTION__);
    size_t index = 0;

    I3GeometryConstPtr geometry;
    if (bookGeometry_) {
      if (!this->currentFrame_)  // obsolete check?
        log_fatal("Trying to book geometry, but the current frame is not set.");
      geometry = this->currentFrame_->template Get<I3GeometryConstPtr>();
      if (!geometry) {
        log_error("%s: No geometry in frame", __PRETTY_FUNCTION__);
        return 0;
      }
    }

    for(typename map_type::const_iterator mapiter = m.begin();
        mapiter != m.end();
        mapiter++)
      {

        AntennaKey key = mapiter->first;
        I3AntennaGeo antgeo;

        if (bookGeometry_) {
          I3AntennaGeoMap::const_iterator geoiter = geometry->antennageo.find(key);
          if (geoiter == geometry->antennageo.end()) {
            log_warn("%s: AntennaKey (%d,%d) not in geometry!", __PRETTY_FUNCTION__,
                     key.GetStationID(), key.GetAntennaID());
            ++nGeometryWarnings;
            if (nGeometryWarnings >= 100)
              log_info("Warned 100 times. Will suppress any further warnings.");
          } else {
            antgeo = geoiter->second;
          }
        }

        int vecindex = 0;
        for (typename map_type::mapped_type::const_iterator veciter = mapiter->second.begin();
             veciter != mapiter->second.end();
             veciter++)
          {
            rows->SetCurrentRow(index);
            rows->Set<int32_t>("station", mapiter->first.GetStationID());
            rows->Set<uint32_t>("antenna", mapiter->first.GetAntennaID());
            if (bookGeometry_) {
              rows->Set<double>("x", antgeo.GetPosition().GetX());
              rows->Set<double>("y", antgeo.GetPosition().GetY());
              rows->Set<double>("z", antgeo.GetPosition().GetZ());
            }
            rows->Set<tableio_size_t>("vector_index", vecindex);

            detail::fill_single_row(converter_, *veciter, rows, this->currentFrame_);
            
            log_trace("Station: %d Antenna: %d",
                  mapiter->first.GetStationID(),
                  mapiter->first.GetAntennaID());
            index++;
            vecindex++;
          }
      }
    // loop over vector
    return index;
  }

private:
  bool bookGeometry_;
  converter_type converter_;
};


#define I3_MAP_ANTKEY_CONVERTER_EXPORT(converter, docstring)           \
  I3CONVERTER_EXPORT(converter, docstring)                             \
  .def(boost::python::init<bool>(boost::python::args("bookGeometry"))) \

#define I3_MAP_ANTKEY_CONVERTER_EXPORT_DEFAULT(converter, docstring)   \
  I3CONVERTER_EXPORT_DEFAULT(converter, docstring)                     \
  .def(boost::python::init<bool>(boost::python::args("bookGeometry"))) \

#endif // TABLEIO_I3MAPANTENNAKEYVECTORCONVERTER_H_INCLUDED
