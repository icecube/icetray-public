#ifndef DEPRECATED_CLASSES_H_INCLUDED
#define DEPRECATED_CLASSES_H_INCLUDED

#include <icetray/serialization.h>
#include <icetray/OMKey.h>
#include <dataclasses/calibration/I3DOMCalibration.h>

struct TWRXTCal{
  double speNetChargeCut;
  LinearFit chargeVsAmplCut;
  double startBinCut;
  double endBinCut;
  double amplitudeCut;
  double invAmplitudeCut;
  
  template <class Archive> void serialize(Archive & ar, unsigned version){
    ar & make_nvp("speNetChargeCut", speNetChargeCut);
    ar & make_nvp("chargeVsAmplCut", chargeVsAmplCut);
    ar & make_nvp("startBinCut", startBinCut);
    ar & make_nvp("endBinCut", endBinCut);
    ar & make_nvp("amplitudeCut", amplitudeCut);
    ar & make_nvp("invAmplitudeCut", invAmplitudeCut);
  }
};
I3_SERIALIZABLE(TWRXTCal);

struct TWRCalibration{    
  double peArea;
  double twrT0;
  TWRXTCal xtcal;
    
  template <class Archive> void serialize(Archive & ar, unsigned version){
    ar & make_nvp("peArea", peArea);
    ar & make_nvp("twrT0",  twrT0);
    ar & make_nvp("XTcal",  xtcal);
  }
};
I3_SERIALIZABLE(TWRCalibration);

struct I3TankCalibrationBackwardsCompat{
  double avgMuonPE;
  double avgMuonRisetime;
  double avgMuonWidth;
  template <class Archive>
  void serialize (Archive& ar, unsigned version){
    ar & make_nvp("AvgMuonPE",avgMuonPE);
    ar & make_nvp("AvgMuonRisetime",avgMuonRisetime);
    ar & make_nvp("AvgMuonWidth",avgMuonWidth);
  }
};
I3_SERIALIZABLE(I3TankCalibrationBackwardsCompat);

struct I3AMANDAOM_XTalk {
  OMKey  receiver;
  double timehigh;
  double timelow;
  double width;
  double threshold;
  
  template <class Archive> 
  void serialize(Archive & ar, unsigned version){
    ar & make_nvp("receiver",receiver);
    ar & make_nvp("timehigh",timehigh);
    ar & make_nvp("timelow",timelow);
    ar & make_nvp("width",width);
    ar & make_nvp("threshold",threshold);
  }
};

I3_SERIALIZABLE(I3AMANDAOM_XTalk);

struct I3AMANDAOMCalibration {

  double adc_a0;
  double adc_beta;
  double tdc_alpha;
  double tdc_beta;
  double tdc_t0;
  int    tdc_pol;
  double tot_beta;
  double tot_tot0;
  int    tot_pol;
  double omo_a, omo_b, omo_c, omo_d, omo_e;  
  double omo_lowlim;
  double omo_highlim;
  double pe_area;  
  std::vector<I3AMANDAOM_XTalk> xtalk_receivers;

  template <class Archive> void serialize(Archive & ar, unsigned version){
    ar & make_nvp("adc_a0_",adc_a0);
    ar & make_nvp("adc_beta",adc_beta);
    ar & make_nvp("tdc_alpha",tdc_alpha);
    ar & make_nvp("tdc_beta",tdc_beta);
    ar & make_nvp("tdc_t0",tdc_t0);
    ar & make_nvp("tdc_pol",tdc_pol);
    ar & make_nvp("tot_beta",tot_beta);
    ar & make_nvp("tot_tot0",tot_tot0);
    ar & make_nvp("tot_pol",tot_pol);
    ar & make_nvp("omo_a",omo_a);
    ar & make_nvp("omo_b",omo_b);
    ar & make_nvp("omo_c",omo_c);
    ar & make_nvp("omo_d",omo_d);
    ar & make_nvp("omo_e",omo_e);
    ar & make_nvp("omo_lowlim",omo_lowlim);
    ar & make_nvp("omo_highlim",omo_highlim);
    ar & make_nvp("pe_area",pe_area);
    ar & make_nvp("xtalk_senders",xtalk_receivers);
  }
};
I3_SERIALIZABLE(I3AMANDAOMCalibration);

#endif //DEPRECATED_CLASSES_H_INCLUDED

