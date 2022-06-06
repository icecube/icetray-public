/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> Last changed by: $LastChangedBy$
 */

// A converter can be made for frame objects of the form map<OMKey, vector<T>> by
// specializing the following templated structs for T. 

// the converters should be written to be indifferent on the state of the I3TableRow.

#ifndef TABLEIO_I3MAPCONVERTER_H_INCLUDED
#define TABLEIO_I3MAPCONVERTER_H_INCLUDED

#include <tableio/I3Converter.h>
#include <dataclasses/I3Map.h>
#include <dataclasses/geometry/I3Geometry.h>
#include <dataclasses/geometry/I3ModuleGeo.h>
#include <icetray/OMKey.h>
#include <dataclasses/ModuleKey.h>
#include <icetray/I3Frame.h>
#include <tableio/converter/container_converter_detail.h>
#include <dataclasses/physics/I3Particle.h>

template <class converter_type,
          typename map_type = I3Map<OMKey, std::vector<typename converter_type::booked_type> >,
          typename frameobject_type = map_type >
class I3MapOMKeyVectorConverter 
  : public I3ConverterImplementation< frameobject_type > 
{
public:
  // Here, we explicitly create different converters for different kinds of arguments (to avoid boost.args errors about ambiguity)
  // For both options
  I3MapOMKeyVectorConverter(bool bookGeometry, std::string bookToParticle)
    : bookGeometry_(bookGeometry),
    bookToParticle_(bookToParticle)
  {}
  // For just one
  I3MapOMKeyVectorConverter(std::string bookToParticle)
    : bookGeometry_(false),
    bookToParticle_(bookToParticle)
  {}
  // For just the other
  I3MapOMKeyVectorConverter(bool bookGeometry)
    : bookGeometry_(bookGeometry),
    bookToParticle_("")
  {}
  // For neither
  I3MapOMKeyVectorConverter()
    : bookGeometry_(false),
    bookToParticle_("")
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
    desc->AddField<int32_t>("string", "", "String number");
    desc->AddField<uint32_t>("om", "", "OM number");
    desc->AddField<uint32_t>("pmt", "", "PMT number");
    if (bookGeometry_) {
      desc->AddField<double>("x", "m", "X coordinate of the DOM");
      desc->AddField<double>("y", "m", "Y coordinate of the DOM");
      desc->AddField<double>("z", "m", "Z coordinate of the DOM");
    }
    if (bookToParticle_ != "") {
      desc->AddField<double>("rho_"+bookToParticle_, "m", "perpendicular distance from DOM to track");
      desc->AddField<double>("l_"+bookToParticle_, "m", "longitudinal distance of the DOM along track from vertex");
      desc->AddField<double>("r_"+bookToParticle_, "m", "distance from DOM to vertex");
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
    if ((bookGeometry_)||(bookToParticle_ != "")) {
      if (!this->currentFrame_)  // obsolete check?
        log_fatal("Trying to book geometry, but the current frame is not set.");
      geometry = this->currentFrame_->template Get<I3GeometryConstPtr>();
      if (!geometry) {
        log_error("%s: No geometry in frame", __PRETTY_FUNCTION__);
        return 0;
      }
    }
    I3ParticleConstPtr track;
    if (bookToParticle_ != "") {
        if (!this->currentFrame_)  // obsolete check?
            log_fatal("Trying to book pulse relationship to track, but the current frame is not set.");
        track = this->currentFrame_->template Get<I3ParticleConstPtr>(bookToParticle_);
        if (!track) {
            log_debug("%s: No Particle %s in frame... but we go on anyway", __PRETTY_FUNCTION__, bookToParticle_.c_str());
            //return 0;
        }
    }

    for(typename map_type::const_iterator mapiter = m.begin();
        mapiter != m.end();
        mapiter++)
      {

        OMKey key = mapiter->first;
        I3OMGeo omgeo;

        if ((bookGeometry_)||(bookToParticle_ != "")) {
          I3OMGeoMap::const_iterator geoiter = geometry->omgeo.find(key);
          if (geoiter == geometry->omgeo.end()) {
            log_warn("%s: OMKey (%d,%d, %d) not in geometry!", __PRETTY_FUNCTION__,
                     key.GetString(), key.GetOM(),
                 static_cast<uint32_t>(mapiter->first.GetPMT()));
            ++nGeometryWarnings;
            if (nGeometryWarnings >= 100)
              log_info("Warned 100 times. Will suppress any further warnings.");
          } else {
            omgeo = geoiter->second;
          }
        }

        int vecindex = 0;
        for (typename map_type::mapped_type::const_iterator veciter = mapiter->second.begin();
             veciter != mapiter->second.end();
             veciter++)
          {
            rows->SetCurrentRow(index);
            rows->Set<int32_t>("string", mapiter->first.GetString());
            rows->Set<uint32_t>("om", mapiter->first.GetOM());
            rows->Set<uint32_t>("pmt", static_cast<uint32_t>(mapiter->first.GetPMT()));
            if (bookGeometry_) {
              rows->Set<double>("x", omgeo.position.GetX());
              rows->Set<double>("y", omgeo.position.GetY());
              rows->Set<double>("z", omgeo.position.GetZ());
            }
            if (bookToParticle_ != "") {
                if (track) {
                  log_debug("%s is present, let's fill it!", bookToParticle_.c_str());
                  // This code stolen/adapted from toprec "GetDistTOAxis" and "GetDistToPlane" -- maybe there's a faster way? in dataclasses?
                  double deltax = omgeo.position.GetX() - track->GetPos().GetX();
                  double deltay = omgeo.position.GetY() - track->GetPos().GetY();
                  double deltaz = omgeo.position.GetZ() - track->GetPos().GetZ();
                  double nx = track->GetDir().GetX();
                  double ny = track->GetDir().GetY();
                  double nz = track->GetDir().GetZ();

                  double abs_x_sq = deltax*deltax + deltay*deltay + deltaz*deltaz;
                  double n_prod_x = nx*deltax + ny*deltay + nz*deltaz;
                  double rho = sqrt(abs_x_sq - n_prod_x * n_prod_x);
                  log_debug("Computing: delta-x y x = %f %f %f", deltax, deltay, deltaz);
                  log_debug("Writing: rho, l, r = %f %f %f", rho, n_prod_x, sqrt(abs_x_sq));

                  rows->Set<double>("rho_"+bookToParticle_, rho);
                  rows->Set<double>("l_"+bookToParticle_, n_prod_x);
                  rows->Set<double>("r_"+bookToParticle_, sqrt(abs_x_sq));
                } else {
                  log_debug("%s missing, but on we go with zeros!", bookToParticle_.c_str());
                  rows->Set<double>("rho_"+bookToParticle_, 0);
                  rows->Set<double>("l_"+bookToParticle_, 0);
                  rows->Set<double>("r_"+bookToParticle_, 0);
                }
            }
            rows->Set<tableio_size_t>("vector_index", vecindex);

            detail::fill_single_row(converter_, *veciter, rows, this->currentFrame_);
            
            log_trace("String: %d OM: %d PMT %d",
                  mapiter->first.GetString(),
                  mapiter->first.GetOM(),
                  static_cast<uint32_t>(mapiter->first.GetPMT()));

            index++;
            vecindex++;
          }
      }
    // loop over vector
    return index;
  }

private:
  bool bookGeometry_;
  std::string bookToParticle_;
  converter_type converter_;
};


template <class converter_type,
typename map_type = I3Map<ModuleKey, std::vector<typename converter_type::booked_type> >,
typename frameobject_type = map_type >
class I3MapModuleKeyVectorConverter 
: public I3ConverterImplementation< frameobject_type > 
{
public:
    // Here, we explicitly create different converters for different kinds of arguments (to avoid boost.args errors about ambiguity)
    // For if both arguments
    I3MapModuleKeyVectorConverter(bool bookGeometry, std::string bookToParticle)
    : bookGeometry_(bookGeometry),
    bookToParticle_(bookToParticle)
    {}
    // For just one
    I3MapModuleKeyVectorConverter(bool bookGeometry)
    : bookGeometry_(bookGeometry),
    bookToParticle_("")
    {}
    // For just the other
    I3MapModuleKeyVectorConverter(std::string bookToParticle)
    : bookGeometry_(false),
    bookToParticle_(bookToParticle)
    {}
    // For neither
    I3MapModuleKeyVectorConverter()
    : bookGeometry_(false),
    bookToParticle_("")
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
        desc->AddField<int32_t>("string", "", "String number");
        desc->AddField<uint32_t>("om", "", "OM number");
        if (bookGeometry_) {
            desc->AddField<double>("x", "m", "X coordinate of the DOM");
            desc->AddField<double>("y", "m", "Y coordinate of the DOM");
            desc->AddField<double>("z", "m", "Z coordinate of the DOM");
        }
        if (bookToParticle_ != "") {
            desc->AddField<double>("rho_"+bookToParticle_, "m", "perpendicular distance from DOM to track");
            desc->AddField<double>("l_"+bookToParticle_, "m", "longitudinal distance of the DOM along track from vertex");
            desc->AddField<double>("r_"+bookToParticle_, "m", "distance from DOM to vertex");
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
        
        I3ModuleGeoMapConstPtr moduleGeo;
        if ((bookGeometry_)||(bookToParticle_ != "")) {
            if (!this->currentFrame_)  // obsolete check?
                log_fatal("Trying to book geometry, but the current frame is not set.");
            moduleGeo = this->currentFrame_->template Get<I3ModuleGeoMapConstPtr>("I3ModuleGeoMap");
            if (!moduleGeo) {
                log_error("%s: No ModuleGeoMap in frame", __PRETTY_FUNCTION__);
                return 0;
            }
        }
        I3ParticleConstPtr track;
        if (bookToParticle_ != "") {
            if (!this->currentFrame_)  // obsolete check?
                log_fatal("Trying to book pulse relationship to track, but the current frame is not set.");
            track = this->currentFrame_->template Get<I3ParticleConstPtr>(bookToParticle_);
            if (!track) {
                log_debug("%s: No Particle %s in frame, but on we go anyway", __PRETTY_FUNCTION__, bookToParticle_.c_str());
                //return 0;
            }
        }

        
        for(typename map_type::const_iterator mapiter = m.begin();
            mapiter != m.end();
            mapiter++)
        {
            
            ModuleKey key = mapiter->first;
            I3Position ompos;
            
            if ((bookGeometry_)||(bookToParticle_ != "")) {
                I3ModuleGeoMap::const_iterator geoiter = moduleGeo->find(key);
                if (geoiter == moduleGeo->end()) {
                    log_warn("%s: ModuleKey (%d,%d) not in geometry!", __PRETTY_FUNCTION__,
                             key.GetString(), key.GetOM());
                    ++nGeometryWarnings;
                    if (nGeometryWarnings >= 100)
                        log_info("Warned 100 times. Will suppress any further warnings.");
                } else {
                    ompos = geoiter->second.GetPos();
                }
            }
            
            int vecindex = 0;
            for (typename map_type::mapped_type::const_iterator veciter = mapiter->second.begin();
                 veciter != mapiter->second.end();
                 veciter++)
            {
                rows->SetCurrentRow(index);
                rows->Set<int32_t>("string", mapiter->first.GetString());
                rows->Set<uint32_t>("om", mapiter->first.GetOM());
                if (bookGeometry_) {
                    rows->Set<double>("x", ompos.GetX());
                    rows->Set<double>("y", ompos.GetY());
                    rows->Set<double>("z", ompos.GetZ());
                }
                if (bookToParticle_ != "") {
                    if (track) {
                        log_debug("%s is present, let's fill it!", bookToParticle_.c_str());
                        // This code stolen/adapted from toprec "GetDistTOAxis" and "GetDistToPlane" -- maybe there's a faster way? in dataclasses?
                        double deltax = ompos.GetX() - track->GetPos().GetX();
                        double deltay = ompos.GetY() - track->GetPos().GetY();
                        double deltaz = ompos.GetZ() - track->GetPos().GetZ();
                        double nx = track->GetDir().GetX();
                        double ny = track->GetDir().GetY();
                        double nz = track->GetDir().GetZ();

                        double abs_x_sq = deltax*deltax + deltay*deltay + deltaz*deltaz;
                        double n_prod_x = nx*deltax + ny*deltay + nz*deltaz;
                        double rho = sqrt(abs_x_sq - n_prod_x * n_prod_x);
                        log_debug("Computing: delta-x y x = %f %f %f", deltax, deltay, deltaz);
                        log_debug("Writing: rho, l, r = %f %f %f", rho, n_prod_x, sqrt(abs_x_sq));

                        rows->Set<double>("rho_"+bookToParticle_, rho);
                        rows->Set<double>("l_"+bookToParticle_, n_prod_x);
                        rows->Set<double>("r_"+bookToParticle_, sqrt(abs_x_sq));
                    } else {
                        log_debug("%s missing, but on we go!", bookToParticle_.c_str());
                        rows->Set<double>("rho_"+bookToParticle_, 0);
                        rows->Set<double>("l_"+bookToParticle_, 0);
                        rows->Set<double>("r_"+bookToParticle_, 0);
                    }
                }
                rows->Set<tableio_size_t>("vector_index", vecindex);
                
                detail::fill_single_row(converter_, *veciter, rows, this->currentFrame_);
                
                log_trace("String: %d OM: %d", mapiter->first.GetString(), mapiter->first.GetOM());
                
                index++;
                vecindex++;
            }
        }
        // loop over vector
        return index;
    }
    
private:
    bool bookGeometry_;
    std::string bookToParticle_;
    converter_type converter_;
};


#define I3_MAP_CONVERTER_EXPORT(converter, docstring)                   \
  I3CONVERTER_EXPORT(converter, docstring)                              \
  .def(boost::python::init<bool, std::string>(boost::python::args("bookGeometry","bookToParticle"))) \
  .def(boost::python::init<bool>(boost::python::args("bookGeometry"))) \
  .def(boost::python::init<std::string>(boost::python::args("bookToParticle")))

#define I3_MAP_CONVERTER_EXPORT_DEFAULT(converter, docstring)           \
  I3CONVERTER_EXPORT_DEFAULT(converter, docstring)                      \
  .def(boost::python::init<bool, std::string>(boost::python::args("bookGeometry","bookToParticle"))) \
  .def(boost::python::init<bool>(boost::python::args("bookGeometry"))) \
  .def(boost::python::init<std::string>(boost::python::args("bookToParticle")))

#endif // TABLEIO_I3MAPCONVERTER_H_INCLUDED
