/**
 *
 * Definition of I3DOMCalibration class
 *
 * copyright  (C) 2004
 * the IceCube collaboration
 * @version $Id$
 * @file I3DOMCalibration.h
 * @date $Date$
 */

#ifndef I3DOMCALIBRATION_H_INCLUDED
#define I3DOMCALIBRATION_H_INCLUDED

#include <string>
#include <map>
#include <vector>
#include <stdint.h>
#include <sstream>
#include <cmath>

#include <boost/math/constants/constants.hpp>

#include <dataclasses/external/CompareFloatingPoint.h>

#include <icetray/I3Units.h>
#include <dataclasses/Utility.h>
#include <icetray/OMKey.h>


static const unsigned i3domcalibration_version_ = 12;
static const unsigned linearfit_version_ = 0;
static const unsigned quadraticfit_version_ = 0;
static const unsigned tauparam_version_ = 0;
static const unsigned SPEChargeDistribution_version_ = 1;

/**
 * @brief A struct to hold a linear fit 
 * A simple struct to define a linear fit:  intercept + slope*x
 */
struct LinearFit
{
  double slope;
  double intercept;
  
  bool operator==(const LinearFit& rhs) const
  {
    return (CompareFloatingPoint::Compare_NanEqual(slope,rhs.slope) && 
        CompareFloatingPoint::Compare_NanEqual(intercept,rhs.intercept));
  }
  bool operator!=(const LinearFit& rhs) const
  {
    return !operator==(rhs);
  }  
    
  template <class Archive>
  void serialize(Archive& ar, unsigned version);
  LinearFit()
  {
    slope = NAN;
    intercept = NAN;
  }

};
I3_CLASS_VERSION(LinearFit, linearfit_version_);

/**
 *  @brief A struct to hold the parameters of a Quadratic fit
 *  A simple struct to define a Quadratic fit:  A + B*x + C*x*x
 */
struct QuadraticFit
{
  double quadFitA;
  double quadFitB;
  double quadFitC;
    
  bool operator==(const QuadraticFit& rhs) const
  {
    return (CompareFloatingPoint::Compare_NanEqual(quadFitA,rhs.quadFitA) &&
        CompareFloatingPoint::Compare_NanEqual(quadFitB,rhs.quadFitB) &&
        CompareFloatingPoint::Compare_NanEqual(quadFitC,rhs.quadFitC));
  }
  bool operator!=(const QuadraticFit& rhs) const
  {
    return !operator==(rhs);
  }  

  template <class Archive>
  void serialize(Archive& ar, unsigned version);
  QuadraticFit()
  {
    quadFitA = NAN;
    quadFitB = NAN;
    quadFitC = NAN;
  }

};
I3_CLASS_VERSION(QuadraticFit, quadraticfit_version_);

/**
 *  @brief: A struct to hold the parameters of an exponential + Gaussian
 *  fit to the SPE charge distribution:
 *  dF/dQ = exp_amp * exp(-Q / exp_width) +
 *              gaus_amp * exp(-0.5 * ((Q - gaus_mean) / gaus_width)^2) /
 *                                                   (sqrt(2*pi) * gaus_width)
 *  where amp_ratio is the ratio of the Gaussian amplitude to the
 *  exponential amplitude.
 */
struct SPEChargeDistribution
{
  template <class Archive> void load(Archive& ar, unsigned version);
  template <class Archive> void save(Archive& ar, unsigned version) const;
  I3_SERIALIZATION_SPLIT_MEMBER();

  SPEChargeDistribution() : exp_amp(NAN),
                            exp_width(NAN),
                            gaus_amp(NAN),
                            gaus_mean(NAN),
                            gaus_width(NAN) { }

  SPEChargeDistribution(double amp_exp,
                        double width_exp,
                        double amp_gaus,
                        double mean_gaus,
                        double width_gaus) :
    exp_amp(amp_exp),
    exp_width(width_exp),
    gaus_amp(amp_gaus),
    gaus_mean(mean_gaus),
    gaus_width(width_gaus) { }

  double exp_amp;
  double exp_width;
  double gaus_amp;
  double gaus_mean;
  double gaus_width;
  
  bool IsValid() const
  {
    // consider valid only if no value is NaN
    return (!std::isnan(exp_amp) &&
        !std::isnan(exp_width) &&
        !std::isnan(gaus_amp) &&
        !std::isnan(gaus_mean) &&
        !std::isnan(gaus_width));
  }
  
  bool operator==(const SPEChargeDistribution& rhs) const
  {
    return (CompareFloatingPoint::Compare_NanEqual(exp_amp, rhs.exp_amp) &&
        CompareFloatingPoint::Compare_NanEqual(exp_width, rhs.exp_width) &&
        CompareFloatingPoint::Compare_NanEqual(gaus_amp, rhs.gaus_amp) &&
        CompareFloatingPoint::Compare_NanEqual(gaus_mean, rhs.gaus_mean) &&
        CompareFloatingPoint::Compare_NanEqual(gaus_width, rhs.gaus_width));
  }
  bool operator!=(const SPEChargeDistribution& rhs) const
  {
    return !operator==(rhs);
  }  
};

I3_CLASS_VERSION(SPEChargeDistribution, SPEChargeDistribution_version_);


/**
 *  @brief A struct to hold the parameters of the toroid time constants parameters
 * for electronic droop correction
 * The two time constants parametrize this correction as a
 * function of temperature (T):
 *   Tau_1(T) = P0 + ( P1/(1+exp(-(T/P2)) ))
 *   Tau_2(T) = P3 + ( P4/(1+exp(-(T/P5)) ))
 * the functions exp(-t/tau1) and exp(-t/tau2) are combined in the TauFrac proportion to reproduce
 * the electronics behaviour through (1-Taufrac)exp(-t/tau1) + TauFrac exp(-t/tau2)
 */

struct TauParam
{
  double P0;
  double P1;
  double P2;
  double P3;
  double P4;
  double P5;
  double TauFrac;
    
  template <class Archive>
  void serialize(Archive& ar, unsigned version);
  
  TauParam():
  P0(NAN),P1(NAN),
  P2(NAN),P3(NAN),
  P4(NAN),P5(NAN),
  TauFrac(NAN){}
  
  TauParam(double p0, double p1, double p2, double p3, double p4, double p5, double tauFrac):
  P0(p0),P1(p1),
  P2(p2),P3(p3),
  P4(p4),P5(p5),
  TauFrac(tauFrac){}
  
  bool operator==(const TauParam& rhs) const
  {
    return (CompareFloatingPoint::Compare_NanEqual(P0,rhs.P0) &&
        CompareFloatingPoint::Compare_NanEqual(P1,rhs.P1) &&
        CompareFloatingPoint::Compare_NanEqual(P2,rhs.P2) &&
        CompareFloatingPoint::Compare_NanEqual(P3,rhs.P3) &&
        CompareFloatingPoint::Compare_NanEqual(P4,rhs.P4) &&
        CompareFloatingPoint::Compare_NanEqual(P5,rhs.P5) &&
        CompareFloatingPoint::Compare_NanEqual(TauFrac,rhs.TauFrac));
  }
  bool operator!=(const TauParam& rhs) const
  {
    return !operator==(rhs);
  }  
};

I3_CLASS_VERSION(TauParam, tauparam_version_);

struct SPETemplate {
  double c, x0, b1, b2;
  SPETemplate():c(NAN), x0(NAN), b1(NAN), b2(NAN){};
    
  SPETemplate(double c, double x0, double b1, double b2):
  c(c), x0(x0), b1(b1), b2(b2){};

  bool operator==(const SPETemplate& rhs) const {
    return (CompareFloatingPoint::Compare_NanEqual(c, rhs.c) &&
	    CompareFloatingPoint::Compare_NanEqual(x0, rhs.x0) &&
	    CompareFloatingPoint::Compare_NanEqual(b1, rhs.b1) && 
	    CompareFloatingPoint::Compare_NanEqual(b2, rhs.b2));
  }
  bool operator!=(const SPETemplate& rhs) const
  {
    return !operator==(rhs);
  }  

};

/**
 * @brief Class that stores the calibration information for a DOM
 * 
 * This class stores the information from the Calibration stream.
 *
 * ATTENTION:
 * Calibration information is set assuming the bin number as it is in
 * the calibration database (reversed in time for bins 0-127).
 * Calibration information is fetched by the time-ordered bin numbers.
 *
 * Nothing here really has units.  Most are fits/offsets that
 * convert values for later storage (at that SHOULD use I3Units)
 *
 * @author Tom McCauley <tpmccauley@lbl.gov>
 * @author Erik Blaufuss \<blaufuss at icecube umd edu\>
 *
 * 
 */

class I3DOMCalibration {
  
 public:
  I3DOMCalibration();
  ~I3DOMCalibration();
  
  /**
   * Get MB Temperature at time of calibration
   */
  double GetTemperature() const { return temperature_; }
  
  /**
   * Set MB Temperature at time of calibration
   */
  void SetTemperature(double temperature);

  /**
   * Get DOMCAL measured PMT transit time
   */
  LinearFit GetTransitTime() const { return pmtTransitTime_; }

  /**
   * Set DOMCAL measured PMT transit time
   */
  void SetTransitTime(LinearFit pmtTransitTime) { pmtTransitTime_ = pmtTransitTime; }
    
  /**
   * Get DOMCAL measured PMT gain/HV relation:  log(10)Gain = slope*log(10)V + intercept
   */
  LinearFit GetHVGainFit() const { return hvGainRelation_; }

  /**
   * Set DOMCAL measured PMT gain/HV relation
   */
  void SetHVGainFit(LinearFit hvGainRelation) { hvGainRelation_ = hvGainRelation; }
    

  /**
   * Get FADC Gain- relation between measured counts and mV.
   */
  double GetFADCGain() const { return fadcGain_; }

  /**
   * Get the FADC baseline in data-taking mode, as measured from beacon launches.
   *
   * Note: The beacon baseline depends implicitly
   * on the front-end bias voltage. If the bias voltage is ever changed, new
   * baselines will have to be collected.
   */
  double GetFADCBeaconBaseline() const { return fadcBeaconBaseline_; }

  /**
   * Get FADC Pedestal- baseline point from which waveforms start.
   */
  LinearFit GetFADCBaselineFit() const { return fadcBaselineFit_ ; }

  /**
   *  Get the FADC intrinsic time offset (in units of time)
   */
  double GetFADCDeltaT() const { return fadcDeltaT_; }
  
  /**
   *  Get the Front End impedance (units are resistance)
   */
  double GetFrontEndImpedance() const { return frontEndImpedance_; }
    
  /**
   * Get parameters for droop correction on the baseline
   */
  TauParam GetTauParameters() const { return tauparameters_; }
  
  /**
   * Set parameters for droop correction on the baseline
   */
  void SetTauParameters(TauParam tauparameters);



  /**
   * Set FADC calibration parameters. Currently the FADC
   * calibration is a work in progress and a moving target
   * so this is only a tentative interface -tpm
   */
  void SetFADCGain(double gain)
  {
    fadcGain_     = gain;
  };

  void SetFADCBeaconBaseline(double bsl)
  {
    fadcBeaconBaseline_ = bsl;
  }

  void SetFADCBaselineFit(LinearFit basefit)
    {
      fadcBaselineFit_ = basefit;
    }
    
  void SetFADCDeltaT(double deltaT)
    {
      fadcDeltaT_ = deltaT;
    }

  void SetFrontEndImpedance(double feImped)
    {
      frontEndImpedance_ = feImped;
    }
    
  /**
   * Get gain and error on gain for ATWD by channel
   */
  double GetATWDGain(unsigned int channel) const;
  /**
   * Set gain and error on gain for ATWD (specified by channel).
   */
  void SetATWDGain(unsigned int channel, double gain);

  /**
   * Get atwd DeltaT by chip
   */
  double GetATWDDeltaT(unsigned int chip) const;
  /**
   * Set atwd DeltaT by chip
   */
  void SetATWDDeltaT(unsigned int chip, double deltat);

  /**
   * Get fit parameters from domcal file \<atwdfreq\> which is 
   * the sampling rate calibration for each ATWD chip 0 or 1 
   */
  QuadraticFit GetATWDFreqFit (unsigned int chip) const;
  
 
  /**
   * Set parameters for sampling rate calibration for each 
   * ATWD chip as a function of the trigger_bias DAC setting
   */
  void SetATWDFreqFit(unsigned int chip, QuadraticFit fitParams);

  /**
   * Get the fit paramater for the bin calibration.
   * This is really the conversion factor from
   * counts to volts.
   */
  double GetATWDBinCalibSlope(unsigned int id,
                              unsigned int channel,
                              unsigned int bin) const;

  /**
   * Sets the parameter for conversion of count to voltage
   * for each ATWD, each ATWD channel, and each ATWD bin.
   */
  void SetATWDBinCalibSlope(unsigned int id,
                            unsigned int channel,
                            unsigned int bin,
                            double slope);

  /**
   *  Get/Set the version of DOMCal.
   */
  std::string GetDOMCalVersion() const
  {
    return domcalVersion_;
  }
  
  void SetDOMCalVersion(std::string version)
  {
    domcalVersion_ = version;
  }

  /**
   * Get the average ATWD baseline in data-taking mode, as measured from beacon launches.
   *
   * Note: The beacon baseline depends implicitly
   * on the front-end bias voltage. If the bias voltage is ever changed, new
   * baselines will have to be collected.
   */
  double GetATWDBeaconBaseline(unsigned int id, unsigned int channel) const;
 
  void SetATWDBeaconBaseline(unsigned int id, unsigned int channel, double bsl);

  /**
   *  Get/Set functions for speDiscrimCalib
   */
  void SetSPEDiscCalib(LinearFit speDiscrimCalib) 
  {
    speDiscrimCalib_ = speDiscrimCalib;
  }

  LinearFit GetSPEDiscCalib() const { return speDiscrimCalib_ ; }  

  /**
   *  Get/Set functions for mpeDiscrimCalib
   */
  void SetMPEDiscCalib(LinearFit mpeDiscrimCalib) 
  {
    mpeDiscrimCalib_ = mpeDiscrimCalib;
  }

  LinearFit GetMPEDiscCalib() const { return mpeDiscrimCalib_ ; }  

  /**
   *  Get/Set functions for pmtDiscrimCalib
   */
  void SetPMTDiscCalib(LinearFit pmtDiscrimCalib) 
  {
    pmtDiscrimCalib_ = pmtDiscrimCalib;
  }

  LinearFit GetPMTDiscCalib() const { return pmtDiscrimCalib_ ; }  

  /**
   *  Get/set for relativeDomEff
   */
  double GetRelativeDomEff() const { return relativeDomEff_ ; }
  void SetRelativeDomEff(double relaeff)
  {
    relativeDomEff_ = relaeff;
  }

  /**
   * Get/set for DOM noise rate
   */
  double GetDomNoiseRate() const { return noiseRate_ ; }
  void SetDomNoiseRate(double noiserate)
  {
    noiseRate_ = noiserate;
  }

  /**
   *  Noise: Thermal DOM noise rate, in Hz, 
   */
  double GetDomNoiseThermalRate() const { return noiseThermalRate_ ; }
  void SetDomNoiseThermalRate(double thermalrate)
  {
    noiseThermalRate_ = thermalrate;
  }

  /**
   *  Noise: Correlated decay rate for DOM in Hz, 
   */
  double GetDomNoiseDecayRate() const { return noiseDecayRate_ ; }
  void SetDomNoiseDecayRate(double decayrate)
  {
    noiseDecayRate_ = decayrate;
  }

  /**
   *  Noise: mean of the lognormal describing scintillation in Log10(dt/ns) for DOM
   */
  double GetDomNoiseScintillationMean() const { return noiseScintillationMean_ ; }
  void SetDomNoiseScintillationMean(double scintillationmean)
  {
    noiseScintillationMean_ = scintillationmean;
  }

  /**
   *  Noise: sigma of the lognormal describing scintillation in Log10(dt/ns) for DOM
   */
  double GetDomNoiseScintillationSigma() const { return noiseScintillationSigma_ ; }
  void SetDomNoiseScintillationSigma(double scintillationsigma)
  {
    noiseScintillationSigma_ = scintillationsigma;
  }


  /**
   *  Noise: mean number of hits from the scintillation lognormal component for DOM
   */
  double GetDomNoiseScintillationHits() const { return noiseScintillationHits_ ; }
  void SetDomNoiseScintillationHits(double scintillationhits)
  {
    noiseScintillationHits_ = scintillationhits;
  }

  //On the assumption that this will be evaulated many times, we copy all data into it. 
  //This makes the object larger but hopefully avoids extra pointer derefences
  class DroopedSPETemplate{
  public:
    SPETemplate pulse;
    struct droopParams{
      SPETemplate pulse;
      double tauFrac, time1, time2;
      
      droopParams(){}
      droopParams(const SPETemplate& templ, 
            double tauFrac, double time1, double time2):
      pulse(templ),tauFrac(tauFrac),time1(time1),time2(time2){}
    } droop;
    bool droopy;
    
    DroopedSPETemplate(const SPETemplate& templ):
    pulse(templ),droopy(false){}
    
    DroopedSPETemplate(const SPETemplate& templ,
               const SPETemplate& droopTempl, 
               double tauFrac, double time1, double time2):
    pulse(templ),droop(droopTempl,tauFrac,time1,time2),droopy(true){}
    
    double operator()(double t){
      if (!droopy)
        return SPEPulseShape(t);
      
      double norm = (1.0 - droop.tauFrac)*droop.time1 + droop.tauFrac*droop.time2;
      double c1 = (1.0 - droop.tauFrac)*droop.time1/norm;
      double c2 = droop.tauFrac*droop.time2/norm;
      
      return SPEPulseShape(t) +
      c1*DroopReactionShape(t, droop.time1) +
      c2*DroopReactionShape(t, droop.time2);
    }
    
    bool operator==(const DroopedSPETemplate& templ) const;
    bool operator!=(const DroopedSPETemplate& templ) const;
    bool operator<(const DroopedSPETemplate& templ) const;
    
  private:
    double SPEPulseShape(double t) const {
      return pulse.c/std::pow(exp(-(t - pulse.x0)/pulse.b1) + 
                         exp((t - pulse.x0)/pulse.b2),8);
    }
    
    double DroopReactionShape(double t, double tau) const{
      return (pulse.c*droop.pulse.c/tau)/
        std::pow(exp(-(t - pulse.x0*droop.pulse.x0)/(pulse.b1*droop.pulse.b1)) + 
            exp((t - pulse.x0*droop.pulse.x0)/(pulse.b2*droop.pulse.b2*tau)),8);
    }
  };

  DroopedSPETemplate DiscriminatorPulseTemplate(bool droopy = false) const;
  DroopedSPETemplate ATWDPulseTemplate(unsigned int channel = 0, bool droopy = false) const;
  DroopedSPETemplate FADCPulseTemplate(bool droopy = false) const;
 
  template <class Archive>
    void serialize(Archive& ar, unsigned version);
    
  enum ToroidType {
    OLD_TOROID = 0,
    NEW_TOROID = 1
  };
  
  ToroidType GetToroidType() const{
    //New toroids should be 50 ohm, while old are 43 ohm.
    return((frontEndImpedance_ > 48*I3Units::ohm) ? NEW_TOROID : OLD_TOROID);
  }
  
  /**
   *  ATWD and FADC-specific corrections to the SPE charge distribution
   */
  double GetMeanATWDCharge() const {return meanATWDCharge_;}
  double GetMeanFADCCharge() const {return meanFADCCharge_;}

  /**
   * In dataclasses we use NaN to denote "invalid" however in the JSON
   * file invalid entries are set to 0.  To cover both cases we check 
   * that it's finite and greater than 0.
   */ 
  bool IsMeanATWDChargeValid() const {
    return ((std::isfinite(meanATWDCharge_)) && (meanATWDCharge_ > 0.));
  }
  bool IsMeanFADCChargeValid() const {
    return ((std::isfinite(meanFADCCharge_)) && (meanFADCCharge_ > 0.));
  }

  void SetMeanATWDCharge(double charge) {
    meanATWDCharge_ = charge;
  }

  void SetMeanFADCCharge(double charge) {
    meanFADCCharge_ = charge;
  }

  SPEChargeDistribution GetCombinedSPEChargeDistribution() const {
    return combinedSPEFit_;
  }

  void SetCombinedSPEChargeDistribution(const SPEChargeDistribution& fit) {
    combinedSPEFit_ = fit;
  }
  
  bool operator==(const I3DOMCalibration& rhs) const
  {    
    return (CompareFloatingPoint::Compare_NanEqual(droopTimeConstants_[0],rhs.droopTimeConstants_[0]) &&
        CompareFloatingPoint::Compare_NanEqual(droopTimeConstants_[1],rhs.droopTimeConstants_[1]) &&
        CompareFloatingPoint::Compare_NanEqual(temperature_,rhs.temperature_) &&
        CompareFloatingPoint::Compare_NanEqual(fadcGain_,rhs.fadcGain_) &&
        fadcBaselineFit_ == rhs.fadcBaselineFit_ &&
        CompareFloatingPoint::Compare_NanEqual(fadcBeaconBaseline_,rhs.fadcBeaconBaseline_) &&
        CompareFloatingPoint::Compare_NanEqual(fadcDeltaT_,rhs.fadcDeltaT_) &&
        CompareFloatingPoint::Compare_NanEqual(frontEndImpedance_,rhs.frontEndImpedance_) &&
        tauparameters_ == rhs.tauparameters_ &&
        CompareFloatingPoint::Compare_NanEqual(ampGains_[0],rhs.ampGains_[0]) &&
        CompareFloatingPoint::Compare_NanEqual(ampGains_[1],rhs.ampGains_[1]) &&
        CompareFloatingPoint::Compare_NanEqual(ampGains_[2],rhs.ampGains_[2]) &&
        atwdFreq_[0] == rhs.atwdFreq_[0] &&
        atwdFreq_[1] == rhs.atwdFreq_[1] &&
        std::equal(&atwdBins_[0][0][0], &atwdBins_[0][0][0] + 
		   2*N_ATWD_CHANNELS*N_ATWD_BINS,
		   &rhs.atwdBins_[0][0][0],
		   CompareFloatingPoint::Compare_NanEqual) &&
        pmtTransitTime_ == rhs.pmtTransitTime_ &&
        hvGainRelation_ == rhs.hvGainRelation_ &&
        domcalVersion_ == rhs.domcalVersion_ &&
        std::equal(&atwdBeaconBaselines_[0][0],&atwdBeaconBaselines_[0][0] +
            2*N_ATWD_CHANNELS, &rhs.atwdBeaconBaselines_[0][0],
            CompareFloatingPoint::Compare_NanEqual) &&
        CompareFloatingPoint::Compare_NanEqual(atwdDeltaT_[0],rhs.atwdDeltaT_[0]) &&
        CompareFloatingPoint::Compare_NanEqual(atwdDeltaT_[1],rhs.atwdDeltaT_[1]) &&
        speDiscrimCalib_ == rhs.speDiscrimCalib_ &&
        mpeDiscrimCalib_ == rhs.mpeDiscrimCalib_ &&
        pmtDiscrimCalib_ == rhs.pmtDiscrimCalib_ &&
        CompareFloatingPoint::Compare_NanEqual(relativeDomEff_,rhs.relativeDomEff_) &&
        CompareFloatingPoint::Compare_NanEqual(noiseRate_,rhs.noiseRate_) &&
        CompareFloatingPoint::Compare_NanEqual(noiseThermalRate_,rhs.noiseThermalRate_) &&
        CompareFloatingPoint::Compare_NanEqual(noiseDecayRate_,rhs.noiseDecayRate_) &&
        CompareFloatingPoint::Compare_NanEqual(noiseScintillationMean_,rhs.noiseScintillationMean_) &&
        CompareFloatingPoint::Compare_NanEqual(noiseScintillationSigma_,rhs.noiseScintillationSigma_) &&
        CompareFloatingPoint::Compare_NanEqual(noiseScintillationHits_,rhs.noiseScintillationHits_) &&
        combinedSPEFit_ == rhs.combinedSPEFit_ &&
        CompareFloatingPoint::Compare_NanEqual(meanATWDCharge_,rhs.meanATWDCharge_) &&
        CompareFloatingPoint::Compare_NanEqual(meanFADCCharge_,rhs.meanFADCCharge_));
  }
  bool operator!=(const I3DOMCalibration& rhs) const
  {
    return !operator==(rhs);
  }

 private:
  static const unsigned int N_ATWD_BINS = 128;
  
  //  Number of ATWD channels is set to 3 (4th ATWD channel doesn't
  //  have DOMCAL now)
  static const unsigned int N_ATWD_CHANNELS = 3;

  double SPEPulseTemplate(double t, const SPETemplate& templ,
    const SPETemplate& droop, bool droopy) const;

  double droopTimeConstants_[2];

  double  temperature_;
 
  /**
   * Gain and pedestal values for FADC
   */
  double fadcGain_;
  LinearFit fadcBaselineFit_;

  /**
   * Pedestal in data-taking mode, as measured from beacon launches.
   * Note : The beacon baseline depends implicitly
   * on the front-end bias voltage. If the bias voltage is ever changed, new
   * baselines will have to be collected.
   */
  double fadcBeaconBaseline_;
  
  /**
   *  FADC inherent time offset (ns)
   */
  double fadcDeltaT_;
  /**
   *  Front-end impedance (Ohms)
   */
  double frontEndImpedance_;
  
  /**
   *   Parameters for droop correction   
   */
  TauParam tauparameters_;
  
  /**
   * Gain for ATWD channels.
   * The key corresponds to the channel (0,1,2)
   */
  //map<unsigned int, double> ampGains_;
  double ampGains_[3];

  /**
   * Linear fit for each ATWD sampling frequency, one for each chip (0,1)
   * As of DOMCAL 5.14, this will be a quadratic fit.  So, use a Quadratic fit
   * and assume a linear fit if quadFitC==NULL.
   */
  //map<unsigned int, QuadraticFit> atwdFreq_;
  QuadraticFit atwdFreq_[2];
  
  /**
   * Slope of the linear fit for the bin calibration
   * i.e. the values needed to convert from counts to voltage
   * for each bin in the ATWD.
   */
  double atwdBins_[2][N_ATWD_CHANNELS][N_ATWD_BINS];
  
  /** 
   *  DOMCAL calculated pmt transit time fit function.
   */

  LinearFit pmtTransitTime_;
  LinearFit hvGainRelation_;

  /**
   * Version of DOMCal used. For now, this only affects the 
   * FE load impedance. It might be useful for FADC calibration
   * as well. Use a std::string since we may have version numbers like
   * 6.1.2, e.g.
   */
  std::string domcalVersion_;

  /**
   *  Dumb-ol-array to hold the average baseline corrections measured from beacon launches.
   *  [atwd chip id (0-1)] [ gain channel(0-2) ]
   *
   * Note : The beacon baseline depends implicitly
   * on the front-end bias voltage. If the bias voltage is ever changed, new
   * baselines will have to be collected.
   */

  double atwdBeaconBaselines_[2][N_ATWD_CHANNELS];

  /**
   *  Store the ATWD time offset from domcal calibration.  the ATWD used in transit time
   *  calibratin will be 0.0, while the other could have up to a few ns offset...
   */

  double atwdDeltaT_[2];

  /**
   * The SPE discriminator calibration.  A linear fit between
   *   DAC(9) value and charge level in pC.
   */
  LinearFit speDiscrimCalib_;

  /**
   * The MPE discriminator calibration.  A linear fit between
   *   DAC(8) value and charge level in pC.
   */
  LinearFit mpeDiscrimCalib_;

  /**
   * A refined SPE discriminator calibration generated using actual PMT pulses.  
   *   A linear fit between DAC(8) value and charge level in pC.
   */
  LinearFit pmtDiscrimCalib_;

  /**
   *  Relative DOM efficiency, normalized to 1.0 for the average dom.
   */
  double relativeDomEff_;

  /**
   *  Measure DOM noise rate, in Hz, 
   *  Deprecated: used in old noise-generator
   */
  double noiseRate_;

  /**
   *  Thermal DOM noise rate, in Hz, 
   */
  double noiseThermalRate_;

  /**
   *  Correlated noise decay rate for DOM in Hz, 
   */
  double noiseDecayRate_;

  /**
   *  Noise: mean of the lognormal describing scintillation in Log10(dt/ns) for DOM
   */
  double noiseScintillationMean_;

  /**
   *  Noise: sigma of the lognormal describing scintillation in Log10(dt/ns) for DOM
   */
  double noiseScintillationSigma_;

  /**
   *  Noise: mean number of hits from the scintillation lognormal component for DOM
   */
  double noiseScintillationHits_;

  /**
   *  Combined-fit SPE distribution function (exponential + Gaussian)
   */
  SPEChargeDistribution combinedSPEFit_;

  /**
   *  ATWD and FADC-specific corrections to the SPE charge distribution
   */
  double meanATWDCharge_;
  double meanFADCCharge_;

  /**
   *  Allow the Diff compression class to directly use private data
   */
  friend class I3DOMCalibrationDiff;
};

typedef std::map<OMKey, I3DOMCalibration> I3DOMCalibrationMap;
I3_POINTER_TYPEDEFS(I3DOMCalibrationMap);

I3_CLASS_VERSION(I3DOMCalibration, i3domcalibration_version_);
I3_POINTER_TYPEDEFS(I3DOMCalibration);

std::ostream& operator<<(std::ostream& oss, const LinearFit& f);
std::ostream& operator<<(std::ostream& oss, const QuadraticFit& f);
std::ostream& operator<<(std::ostream& oss, const SPEChargeDistribution& spe);
std::ostream& operator<<(std::ostream& oss, const TauParam& p);
std::ostream& operator<<(std::ostream& oss, const I3DOMCalibration& c);
std::ostream& operator<<(std::ostream& oss, const I3DOMCalibrationMap& m);

#endif //I3DOMCALIBRATION_H_INCLUDED
