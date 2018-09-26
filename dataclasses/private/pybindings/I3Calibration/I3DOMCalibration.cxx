//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id$
//
//   This file is part of IceTray.
//
//   IceTray is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
//   IceTray is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <dataclasses/calibration/I3DOMCalibration.h>
#include <icetray/python/copy_suite.hpp>
#include <icetray/python/indexed_property.hpp>
#include <icetray/python/boost_serializable_pickle_suite.hpp>
#include <icetray/python/dataclass_suite.hpp>
#include <icetray/python/stream_to_string.hpp>

using namespace boost::python;

template<typename theType>
std::string to_str(const theType& theStr){
    std::ostringstream oss;
    oss << theStr << std::flush;
    return oss.str();
}

std::string to_str_I3DOMCalibration(const I3DOMCalibration& self){
  return to_str<I3DOMCalibration>(self); };
std::string to_str_LinearFit(const LinearFit& self){
  return to_str<LinearFit>(self); };
std::string to_str_QuadraticFit(const QuadraticFit& self){
  return to_str<QuadraticFit>(self); };
std::string to_str_TauParam(const TauParam& self){
  return to_str<TauParam>(self); };
std::string to_str_SPEChargeDistribution(const SPEChargeDistribution& self){
  return to_str<SPEChargeDistribution>(self); };
std::string to_str_I3DOMCalibrationMap(const I3DOMCalibrationMap& self){
  return to_str<I3DOMCalibrationMap>(self); };

void register_I3DOMCalibration()
{

  {

    scope outer = 
      class_<I3DOMCalibration, boost::shared_ptr<I3DOMCalibration> >("I3DOMCalibration")
      #define I3DOMCALPROPS (Temperature)(TransitTime)(HVGainFit)(FADCGain)           \
                            (FADCBaselineFit)(FADCBeaconBaseline)(FrontEndImpedance)  \
                            (TauParameters)(FADCGain)(FADCDeltaT)(DOMCalVersion)      \
                            (SPEDiscCalib)      \
                            (MPEDiscCalib)(PMTDiscCalib)(DomNoiseRate)(RelativeDomEff)\
                            (DomNoiseThermalRate)(DomNoiseDecayRate)      \
                            (DomNoiseScintillationMean)(DomNoiseScintillationSigma)   \
                            (DomNoiseScintillationHits)(MeanATWDCharge)(MeanFADCCharge)\
                            (CombinedSPEChargeDistribution)
      BOOST_PP_SEQ_FOR_EACH(WRAP_PROP, I3DOMCalibration, I3DOMCALPROPS)
      #undef I3DOMCALPROPS
      #define EVIL_PROPS (ATWDBeaconBaseline)(ATWDDeltaT) \
                         (ATWDFreqFit)(ATWDGain)(ATWDBinCalibSlope)
      BOOST_PP_SEQ_FOR_EACH(WRAP_EVIL_PROP, I3DOMCalibration, EVIL_PROPS)
      #undef EVIL_PROPS
      .add_property("toroid_type", &I3DOMCalibration::GetToroidType)
      .add_property("is_mean_atwd_charge_valid", &I3DOMCalibration::IsMeanATWDChargeValid)
      .add_property("is_mean_fadc_charge_valid", &I3DOMCalibration::IsMeanFADCChargeValid)
      .def("atwd_pulse_template", &I3DOMCalibration::ATWDPulseTemplate)
      .def("fadc_pulse_template", &I3DOMCalibration::FADCPulseTemplate)
      .def("discriminator_pulse_template", &I3DOMCalibration::DiscriminatorPulseTemplate)
      .def("__str__", to_str_I3DOMCalibration)
      .def("__str__", to_str_LinearFit)
      .def("__str__", to_str_QuadraticFit)
      .def("__str__", to_str_TauParam)
      .def("__str__", to_str_SPEChargeDistribution)
      .def("__str__", to_str_I3DOMCalibrationMap)
      .def(dataclass_suite<I3DOMCalibration>())
      ;

    enum_<I3DOMCalibration::ToroidType>("ToroidType")
      .value("OLD_TOROID", I3DOMCalibration::OLD_TOROID)
      .value("NEW_TOROID", I3DOMCalibration::NEW_TOROID)
      .export_values()
      ;

  }

  class_<I3DOMCalibrationMap, 
         I3DOMCalibrationMapPtr>("I3DOMCalibrationMap")
    .def(dataclass_suite<I3DOMCalibrationMap>())
    ;
}
