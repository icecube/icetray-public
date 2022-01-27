
//
//  $Id$
//
//
#include <map>
#include <icetray/serialization.h>
#include <dataclasses/calibration/I3DOMCalibration.h>
#include <icetray/I3Units.h>

#include <gsl/gsl_integration.h>

I3DOMCalibration::~I3DOMCalibration() { } 

I3DOMCalibration::I3DOMCalibration()
  : temperature_(NAN),
    fadcGain_(NAN),
    fadcDeltaT_(NAN),
    frontEndImpedance_(NAN),
    domcalVersion_("unknown"),
    relativeDomEff_(NAN),
    noiseRate_(NAN),
    noiseThermalRate_(NAN),
    noiseDecayRate_(NAN),
    noiseScintillationMean_(NAN),
    noiseScintillationSigma_(NAN),
    noiseScintillationHits_(NAN),
    meanATWDCharge_(NAN),
    meanFADCCharge_(NAN)
{
  fadcBeaconBaseline_ = NAN;
  atwdBeaconBaselines_[0][0] = NAN;
  atwdBeaconBaselines_[0][1] = NAN;
  atwdBeaconBaselines_[0][2] = NAN;
  atwdBeaconBaselines_[1][0] = NAN;
  atwdBeaconBaselines_[1][1] = NAN;
  atwdBeaconBaselines_[1][2] = NAN;
  ampGains_[0] = NAN;
  ampGains_[1] = NAN;
  ampGains_[2] = NAN;
  atwdDeltaT_[0] = NAN;
  atwdDeltaT_[1] = NAN;
  droopTimeConstants_[0] = NAN;
  droopTimeConstants_[1] = NAN;
  for(unsigned i = 0; i < 2; ++i)
    for(unsigned j = 0; j < N_ATWD_CHANNELS; ++j)
      for(unsigned k = 0; k < N_ATWD_BINS; ++k)
	atwdBins_[i][j][k] = NAN;
}

double
I3DOMCalibration::GetATWDBinCalibSlope(unsigned int id,
                                       unsigned int channel,
                                       unsigned int bin) const{
  assert(id<2);
  assert(channel<N_ATWD_CHANNELS);
  assert(bin<N_ATWD_BINS);
  return(atwdBins_[id][channel][bin]);
}

void
I3DOMCalibration::SetATWDBinCalibSlope(unsigned int id,
                                       unsigned int channel,
                                       unsigned int bin,
                                       double slope){
  assert(id<2);
  assert(channel<N_ATWD_CHANNELS);
  assert(bin<N_ATWD_BINS);
  atwdBins_[id][channel][(N_ATWD_BINS-1)-bin]=slope;
}

QuadraticFit
I3DOMCalibration::GetATWDFreqFit (unsigned int chip) const
{
  //map<unsigned int, QuadraticFit>::const_iterator iter = atwdFreq_.find(chip);
  
  if(chip == 0 || chip ==1 )
    {
      return atwdFreq_[chip];
    }
  else
    {
      log_fatal("Frequency calib not found for ATWD chip in I3DOMCalibration");
    }
}

void 
I3DOMCalibration::SetATWDFreqFit (unsigned int chip, 
          QuadraticFit fitParams)
{
  if(chip == 0 || chip ==1 )
    {
      atwdFreq_[chip] = fitParams;
    }
  else
    {
      log_fatal("Invalid chip specified in I3DOMCalibration::SetATWDFreqFit");
    }
}

void 
I3DOMCalibration::SetATWDGain (unsigned int channel, 
             double gain) 
{
  if(channel == 0 || channel == 1 || channel == 2)
    {
      ampGains_[channel]=gain;
    }
  else
    {
      log_fatal("Invalid channel specified in I3DOMCalibration::SetATWDGain");
    }
}

double 
I3DOMCalibration::GetATWDGain (unsigned int channel) const
{
  if(channel == 0 || channel == 1 || channel == 2)
    {
      return ampGains_[channel];
    }
  else
    {
      log_fatal("Invalid ATWD channel requested I3DOMCalibration::GetATWDGain");
    }
}

void 
I3DOMCalibration::SetATWDDeltaT (unsigned int chip, 
                                 double deltat) 
{
  if(chip == 0 || chip == 1)
    {
      atwdDeltaT_[chip]=deltat;
    }
  else
    {
      log_fatal("Invalid chip id specified in I3DOMCalibration::SetATWDDeltaT");
    }
}

double 
I3DOMCalibration::GetATWDDeltaT (unsigned int chip) const
{
  if(chip == 0 || chip == 1)
    {
      return atwdDeltaT_[chip];
    }
  else
    {
      log_fatal("Invalid chip id requested I3DOMCalibration::GetATWDDeltaT");
    }
}

double
I3DOMCalibration::GetATWDBeaconBaseline(unsigned int id, unsigned int channel) const
{
  if ((id == 0 || id ==1) && 
      ( channel == 0 || channel == 1 || channel == 2)) {
    return atwdBeaconBaselines_[id][channel];
  } else {
    log_fatal("Invalid id, channel specified for GetATWDBeaconBaseline");
  }
}

void
I3DOMCalibration::SetATWDBeaconBaseline(unsigned int id, unsigned int channel, double bsl)
{
  if ((id == 0 || id ==1) && 
      ( channel == 0 || channel == 1 || channel == 2)) {
    atwdBeaconBaselines_[id][channel] = bsl;
  } else {
    log_fatal("Invalid id, channel specified for SetATWDBeaconBaseline");
  }
}

void
I3DOMCalibration::SetTemperature(double temperature)
{
  temperature_ = temperature;
  SetTauParameters(tauparameters_);
}

void
I3DOMCalibration::SetTauParameters(TauParam tauparameters)
{
  tauparameters_ = tauparameters;
  
  droopTimeConstants_[0] = tauparameters_.P0 + tauparameters_.P1 /
      (1.0 + exp(-((temperature_-273.0)/tauparameters_.P2)));
  droopTimeConstants_[1] = tauparameters_.P3 + tauparameters_.P4 /
      (1.0 + exp(-((temperature_-273.0)/tauparameters_.P5)));
}


/*
 * Pulse template functions for use in simulation and reconstruction.
 * 
 * Per discussion with C. Wendt Jan. 13, 2011, his FADC fits are offset 50 ns
 * and ATWD fits offset 5 ns. Other parameters from web page at 
 * http://www.icecube.wisc.edu/~chwendt/dom-spe-waveform-shape/
 * or, in the case of the denominator of c, the result of numerical integrals.
 *
 * Templates for ATWD channels 1 and 2 were bootstrapped from the channel 0
 * using the method described here:
 * http://www.icecube.wisc.edu/~jvansanten/docs/atwd_pulse_templates/
 */

const double causalityShift = -11.5;  /* Nanoseconds from peak center to photon */

const SPETemplate ATWDNewToroidTemplate[3] = {
  /* Channel 0: fit from SPE pulses */
  SPETemplate(17.899 / 14.970753076313095,	      
	      -4.24 - 5 - causalityShift,
	      5.5,
	      42),
  /* Channel 1: bootstrapped from channel 0 */
  SPETemplate(1.6581978,	      
	      -11.70227755 - causalityShift,
	      5.4664884,
	      36.22319705),
  /* Channel 2: bootstrapped from channel 1 */
  SPETemplate(0.70944364,    
	      -10.58782492- causalityShift,
	      3.48330553,
	      42.10873959)
};

const SPETemplate ATWDNewToroidDroopTemplate[3] = {
  SPETemplate(-0.8644871211757873,    
	      -0.39712728498041222,
	      2.2153931795324807e-08,
	      0.18265408524009966),
  SPETemplate(-0.60714457126191879,    
	      1.0708609673531526,
	      0.85478360796100328,
	      0.22084066752348605),
  SPETemplate(-1.4510165738141465,    
	      -0.29659623453192685,
	      7.5567807067886802e-09,
	      0.18209846421412432),
};

const SPETemplate ATWDOldToroidTemplate[3] = {
  /* Channel 0: fit from SPE pulses */
  SPETemplate(15.47 / 13.292860653948139,    
	      -3.929 - 5 - causalityShift,
	      4.7,
	      39.),
  /* Channel 1: bootstrapped from channel 0 */
  SPETemplate(2.07399312,    
	      -10.95781298 - causalityShift,
	      4.86019733,
	      30.74826947),
  /* Channel 2: bootstrapped from channel 1 */
  SPETemplate(1.35835821,    
	      -9.68624195 - causalityShift,
	      3.5016398,
	      30.96897853),
};

const SPETemplate ATWDOldToroidDroopTemplate[3] = {
  SPETemplate(-0.87271352029389926,	      
	      -0.37445896923019595,
	      0.05292192451474604,
	      0.2015123032569355),	      
  SPETemplate(-0.48448879003182993,	      
	      0.035060687415361419,
	      0.044493411456291751,
	      0.25894387769482058),	      
  SPETemplate(-0.74959447466950724,	      
	      0.16580945347622786,
	      0.055065176963265461,
	      0.25173422056591982),	      
};

const SPETemplate FADCTemplate(25.12 / 71.363940160184669,			       
			       61.27 - 50 - causalityShift,
			       30.,
			       186.);

const SPETemplate FADCDroopTemplate(-2.8837584956162883,  
				    0.57888025049064207,
				    0.81965713180496758,
				    0.04299648444652391);

I3DOMCalibration::DroopedSPETemplate
I3DOMCalibration::DiscriminatorPulseTemplate(bool droopy) const
{
  // Just give the ATWD0 template for now. Better values should be measured
  // later.

  return ATWDPulseTemplate(0, droopy);
}

I3DOMCalibration::DroopedSPETemplate
I3DOMCalibration::ATWDPulseTemplate(unsigned channel, bool droopy) const
{
  if (channel > 2)
    log_fatal("Unknown ATWD channel %u", channel);
  
  const ToroidType toroidType = GetToroidType();
  switch (toroidType){
    case OLD_TOROID: 
      if(!droopy)
        return(DroopedSPETemplate(ATWDOldToroidTemplate[channel]));
      return(DroopedSPETemplate(ATWDOldToroidTemplate[channel],
                                ATWDOldToroidDroopTemplate[channel],
                                tauparameters_.TauFrac,
                                droopTimeConstants_[0],
                                droopTimeConstants_[1]));
    case NEW_TOROID:
      if(!droopy)
        return(DroopedSPETemplate(ATWDNewToroidTemplate[channel]));
      return(DroopedSPETemplate(ATWDNewToroidTemplate[channel],
                                ATWDNewToroidDroopTemplate[channel],
                                tauparameters_.TauFrac,
                                droopTimeConstants_[0],
                                droopTimeConstants_[1]));
    default:
      log_fatal("Unknown toroid type %d", toroidType);
  }
}

I3DOMCalibration::DroopedSPETemplate
I3DOMCalibration::FADCPulseTemplate(bool droopy) const
{
  if(!droopy)
    return(DroopedSPETemplate(FADCTemplate));
  return(DroopedSPETemplate(FADCTemplate,
                            FADCDroopTemplate,
                            tauparameters_.TauFrac,
                            droopTimeConstants_[0],
                            droopTimeConstants_[1]));
}

bool I3DOMCalibration::DroopedSPETemplate::operator==(const DroopedSPETemplate& templ) const
{
  return(pulse.c==templ.pulse.c && 
         pulse.x0==templ.pulse.x0 && 
         pulse.b1==templ.pulse.b1 && 
         pulse.b2==templ.pulse.b2 && 
         droopy==templ.droopy &&
         droop.pulse.c==templ.droop.pulse.c && 
         droop.pulse.x0==templ.droop.pulse.x0 && 
         droop.pulse.b1==templ.droop.pulse.b1 && 
         droop.pulse.b2==templ.droop.pulse.b2 &&
         droop.tauFrac==templ.droop.tauFrac &&
         droop.time1==templ.droop.time1 &&
         droop.time2==templ.droop.time2);
}

bool I3DOMCalibration::DroopedSPETemplate::operator!=(const DroopedSPETemplate& templ) const{
  return !operator==(templ);
}

bool I3DOMCalibration::DroopedSPETemplate::operator<(const DroopedSPETemplate& templ) const
{
#define DSPET_compare_member(member) \
  if(member<templ.member) return(true); \
  if(member>templ.member) return(false);
  
  DSPET_compare_member(pulse.c);
  DSPET_compare_member(pulse.x0);
  DSPET_compare_member(pulse.b1);
  DSPET_compare_member(pulse.b2);
  //we'll say that non-droopy templates are smaller than droopy templates
  if(droopy!=templ.droopy)
    return(!droopy);
  if(!droopy)
    return(false); //neither is droopy, so the templates are equal
  //both are droopy, so we go on comparing the components of the droop
  DSPET_compare_member(droop.pulse.c);
  DSPET_compare_member(droop.pulse.x0);
  DSPET_compare_member(droop.pulse.b1);
  DSPET_compare_member(droop.pulse.b2);
  DSPET_compare_member(droop.tauFrac);
  DSPET_compare_member(droop.time1);
  DSPET_compare_member(droop.time2);
  return(false); //templates are equal
  
#undef DSPET_compare_member
}

//
// these are some beeeeautiful serialization functions.
//
template <class Archive>
void 
LinearFit::serialize(Archive& ar, unsigned version)
{
  if (version>linearfit_version_)
    log_fatal("Attempting to read version %u from file but running version %u of LinearFit class.",version,linearfit_version_);

  ar & make_nvp("slope",slope);
  ar & make_nvp("intercept",intercept);
}

I3_SERIALIZABLE(LinearFit);

template <class Archive>
void 
QuadraticFit::serialize(Archive& ar, unsigned version)
{
  if (version>quadraticfit_version_)
    log_fatal("Attempting to read version %u from file but running version %u of QuadraticFit class.",version,quadraticfit_version_);

  ar & make_nvp("quadraticA",quadFitA);
  ar & make_nvp("quadraticB",quadFitB);
  ar & make_nvp("quadraticC",quadFitC);
}

I3_SERIALIZABLE(QuadraticFit);

template <class Archive>
void
SPEChargeDistribution::save(Archive& ar, unsigned version) const
{	
  ar & make_nvp("Exp2Amp", exp2_amp); 		
  ar & make_nvp("Exp2Width", exp2_width);
  ar & make_nvp("Exp1Amp", exp1_amp);
  ar & make_nvp("Exp1Width", exp1_width);
  ar & make_nvp("CompensationFactor", compensation_factor);
  ar & make_nvp("SLCGausMean", SLC_gaus_mean);
  ar & make_nvp("GausAmp", gaus_amp);
  ar & make_nvp("GausMean", gaus_mean);
  ar & make_nvp("GausWidth", gaus_width);
}

template <class Archive>
void
SPEChargeDistribution::load(Archive& ar, unsigned version)
{
  if (version>SPEChargeDistribution_version_)
    log_fatal("Attempting to read version %u from file but running version %u of SPEChargeDistribution class.",version,SPEChargeDistribution_version_);

  if (version >= 2){
    ar & make_nvp("Exp2Amp", exp2_amp);
    ar & make_nvp("Exp2Width", exp2_width);
    ar & make_nvp("Exp1Amp", exp1_amp);
    ar & make_nvp("Exp1Width", exp1_width);
    ar & make_nvp("CompensationFactor", compensation_factor);
    ar & make_nvp("SLCGausMean", SLC_gaus_mean);
  }else{
    ar & make_nvp("ExpAmp", exp2_amp);
    ar & make_nvp("ExpWidth", exp2_width);
    exp1_amp=0.;
    exp1_width=1.;
    compensation_factor  = 1.;
    SLC_gaus_mean = 1.;
  }
  ar & make_nvp("GausAmp", gaus_amp);
  ar & make_nvp("GausMean", gaus_mean);
  ar & make_nvp("GausWidth", gaus_width);
}

I3_SPLIT_SERIALIZABLE(SPEChargeDistribution);

namespace GSL{

///\brief Compute a one-dimensional integral using GSL.
///\param f Function to integrate.
///\param a Lower integration limit.
///\param b Upper integration limit.
///\param acc Accuracy parameter.
///\param max_iter Maximum number of iterations to perform the integral.
template<typename FunctionType>
double integrate(FunctionType&& f, double a, double b, double rtol=1e-7, unsigned int max_iter=10000, size_t memory_alloc=10000){
  using IntegrateWorkspace=std::unique_ptr<gsl_integration_workspace, void(*)(gsl_integration_workspace*)>;
  IntegrateWorkspace ws(gsl_integration_workspace_alloc(memory_alloc), &gsl_integration_workspace_free);

  using FPtr=decltype(&f);
  double (*wrapper)(double,void*)=[](double x, void* params)->double{
    auto& f=*static_cast<FPtr>(params);
    return(f(x));
  };

  double result, error;
  gsl_function F;
  F.function = wrapper;
  F.params = const_cast<typename std::remove_const<typename std::remove_reference<decltype(f)>::type>::type*>(&f);

  gsl_integration_qag(&F, a, b, 0, rtol, max_iter, GSL_INTEG_GAUSS15, ws.get(), &result, &error);

  return(result);
}

} //namespace GSL

double SPEChargeDistribution::ComputeMeanCharge() const{
  //Analytic caclulation which neglects the residual correction factor
  //return exp1_amp*std::pow(exp1_width,2)+exp2_amp*std::pow(exp2_width,2)
  //  +gaus_amp*std::sqrt(M_PI/2)*gaus_mean*gaus_width*(1+std::erf(gaus_mean/(gaus_width*std::sqrt(2))))
  //  +gaus_amp*std::pow(gaus_width,2)*std::exp(-0.5*std::pow(gaus_mean/gaus_width,2));
  
  //This might seems like a very loose tolerance, but the GSL integrator tends to
  //fail for any tighter setting.
  //However, it also tends to substantially overestimate its error, at least for
  //these functions, as checking all charge distributions in
  //GeoCalibDetectorStatus_2020.Run134142.Pass2_V0.i3.gz against another, less touchy
  //algorithm indicates that it produces an average relative error in the computed
  //mean of only 4.2e-06, and the worst observed error is 2.4e-4.  
  const double int_tol = 1e-3;
  double norm = GSL::integrate(*this, 0, 10*gaus_mean, int_tol);
  auto moment_integrand = [this](double x){ return x*this->operator()(x); };
  double mom1 = GSL::integrate(moment_integrand, 0, 10*gaus_mean, int_tol);
  return mom1/norm;
}

template <class Archive>
void 
TauParam::serialize(Archive& ar, unsigned version)
{
  if (version>tauparam_version_)

    log_fatal("Attempting to read version %u from file but running version %u of TuaParam class.",version,tauparam_version_);

  ar & make_nvp("P0",P0);
  ar & make_nvp("P1",P1);
  ar & make_nvp("P2",P2);
  ar & make_nvp("P3",P3);
  ar & make_nvp("P4",P4);
  ar & make_nvp("P5",P5);
  ar & make_nvp("TauFrac",TauFrac);
}

I3_SERIALIZABLE(TauParam);


template <class Archive>
void 
I3DOMCalibration::serialize(Archive& ar, unsigned version)
{
  if (version>i3domcalibration_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3DOMCalibration class.",version,i3domcalibration_version_);
  ar & make_nvp("temperature",temperature_);
  ar & make_nvp("fadcGain",fadcGain_);
  ar & make_nvp("fadcBaseline",fadcBaselineFit_);

  if(version > 0)
  {
    ar & make_nvp("fadcDeltaT", fadcDeltaT_);
  }
  if(version < 4)
    {
      //In this case, we need to convert the old map<> struct to simple array
      std::map<unsigned int, double> ampGains_temp;
      ar & make_nvp("ampGains",ampGains_temp);
      std::map<unsigned int, double>::iterator iter = ampGains_temp.begin();
      while(iter != ampGains_temp.end())
      {
        if(iter->first < 3)
          {
            ampGains_[iter->first] = iter->second;
          }
        else
          {
            log_fatal("Invalid channel for ampGains in I3DOMCalibration::serialize");
          }
              iter++;
      }
    }
  else
    {
      ar & make_nvp("ampGains",ampGains_);
    }
  if(version < 4)
    {
      //In this case, we need to convert the old map<> struct to simple array
      std::map<unsigned int, QuadraticFit> atwdFreq_temp;
      ar & make_nvp("atwdFreq",atwdFreq_temp);
      std::map<unsigned int, QuadraticFit>::iterator iter = atwdFreq_temp.begin();
      while(iter !=  atwdFreq_temp.end())
      {
        if(iter->first < 2)
          {
            atwdFreq_[iter->first] = iter->second;
          }
        else
          {
            log_fatal("Invalid chip number for atwdFreq in I3DOMCalibration::serialize");
          }
              iter++;
      }
    }
  else
    {
      ar & make_nvp("atwdFreq",atwdFreq_);
    }
  if(version < 4)
    {
      //In this case, we need to convert the old map<> struct to simple array
      std::map<unsigned, std::map<unsigned, LinearFit> > atwdBin0_temp;
      std::map<unsigned, std::map<unsigned, LinearFit> > atwdBin1_temp;
      ar & make_nvp("atwd0BinParameters",atwdBin0_temp);
      ar & make_nvp("atwd1BinParameters",atwdBin1_temp);
      //  For atwdBin0:
      //first iterate over the outer map, this is over all channels (0-2)
      std::map<unsigned, std::map<unsigned, LinearFit> >::iterator iter_nch = atwdBin0_temp.begin();
      while(iter_nch != atwdBin0_temp.end())
      {
        if(iter_nch->first <3)
          {
            // now for each channel, iterate over all bins (0-127)
            std::map<unsigned, LinearFit>::iterator iter_nbin = iter_nch->second.begin();
            while(iter_nbin != iter_nch->second.end() )
        {
          if(iter_nbin->first<128)
            {
              atwdBins_[0][iter_nch->first][iter_nbin->first] = iter_nbin->second.slope;
            }
          else
            {
              log_fatal("Invalid bin for atwdBin_ in I3DOMCalibration::serialize");
            }
                      iter_nbin++;
        }
          }
        else
          {
            log_fatal("Invalid channel for atwdBin_ in I3DOMCalibration::serialize");
          }
              iter_nch++;
      }
      //  For atwdBin1:
      //first iterate over the outer map, this is over all channels (0-2)
      iter_nch = atwdBin1_temp.begin();
      while(iter_nch != atwdBin1_temp.end())
      {
        if(iter_nch->first <3)
          {
            // now for each channel, iterate over all bins (0-127)
            std::map<unsigned, LinearFit>::iterator iter_nbin = iter_nch->second.begin();
            while(iter_nbin != iter_nch->second.end() )
        {
          if(iter_nbin->first<128)
            {
              atwdBins_[1][iter_nch->first][iter_nbin->first] = iter_nbin->second.slope;
            }
          else
            {
              log_fatal("Invalid bin for atwdBin_ in I3DOMCalibration::serialize");
            }
                      iter_nbin++;
        }
          }
        else
          {
            log_fatal("Invalid channel for atwdBin_ in I3DOMCalibration::serialize");
          }
              iter_nch++;
      }
    }
  else if(version<11)
    {
      //interleave the per-bin slope information with space
      //for the old intercept entries
      LinearFit oldATWDBins[N_ATWD_CHANNELS][N_ATWD_BINS];
      
      for(unsigned int i=0; i<N_ATWD_CHANNELS; i++){
        for(unsigned int j=0; j<N_ATWD_BINS; j++){
          oldATWDBins[i][j].slope=atwdBins_[0][i][j];
          oldATWDBins[i][j].intercept=0;
        }
      }
      ar & make_nvp("atwd0BinParameters",oldATWDBins);
      for(unsigned int i=0; i<N_ATWD_CHANNELS; i++){
        for(unsigned int j=0; j<N_ATWD_BINS; j++){
          atwdBins_[0][i][j]=oldATWDBins[i][j].slope;
        }
      }
      
      for(unsigned int i=0; i<N_ATWD_CHANNELS; i++){
        for(unsigned int j=0; j<N_ATWD_BINS; j++){
          oldATWDBins[i][j].slope=atwdBins_[1][i][j];
          oldATWDBins[i][j].intercept=0;
        }
      }
      ar & make_nvp("atwd1BinParameters",oldATWDBins);
      for(unsigned int i=0; i<N_ATWD_CHANNELS; i++){
        for(unsigned int j=0; j<N_ATWD_BINS; j++){
          atwdBins_[1][i][j]=oldATWDBins[i][j].slope;
        }
      }
    }
  else
    {
      ar & make_nvp("atwdBinParameters",atwdBins_);
    }
  ar & make_nvp("pmtTransitTime",pmtTransitTime_);
  ar & make_nvp("hvGainRelation",hvGainRelation_);
  if(version > 1)
  {
    ar & make_nvp("frontEndImpedance", frontEndImpedance_);
  }

  if (version > 1)
    {
      ar & make_nvp("domcalVersion", domcalVersion_);
    }
  if (version > 2)
    {
      ar & make_nvp("tauparameters", tauparameters_);
      SetTauParameters(tauparameters_);
      if (version < 11)
        {
          double atwdBaselines[2][N_ATWD_CHANNELS][N_ATWD_BINS];
          double atwdResponseWidth, fadcResponseWidth;
          ar & make_nvp("ATWDBaselines", atwdBaselines);
          ar & make_nvp("ATWDResponseWidth", atwdResponseWidth);
          ar & make_nvp("FADCResponseWidth", fadcResponseWidth);
        }
    }

  if (version > 4)
    {
      ar & make_nvp("atwdDeltaT", atwdDeltaT_);
    }
  if (version > 5)
    {
      ar & make_nvp("speDiscrimCalib", speDiscrimCalib_);
      ar & make_nvp("mpeDiscrimCalib", mpeDiscrimCalib_);
    }
  if (version > 6)
    {
      ar & make_nvp("pmtDiscrimCalib", pmtDiscrimCalib_);
    }
  if (version > 7)
    {
      ar & make_nvp("relativeDomEff", relativeDomEff_);
      ar & make_nvp("noiseRate", noiseRate_);
    }
  if (version > 8)
    {
      ar & make_nvp("ATWDBeaconBaselines", atwdBeaconBaselines_);
      ar & make_nvp("FADCBeaconBaselines", fadcBeaconBaseline_);
    }

  if (version > 9)
    {
      ar & make_nvp("noiseThermalRate", noiseThermalRate_);
      ar & make_nvp("noiseDecayRate", noiseDecayRate_);
      ar & make_nvp("noiseScintillationMean", noiseScintillationMean_);
      ar & make_nvp("noiseScintillationSigma", noiseScintillationSigma_);
      ar & make_nvp("noiseScintillationHits", noiseScintillationHits_);
    }
  if (version > 10)
    {
      ar & make_nvp("combinedSPEFit", combinedSPEFit_);
      ar & make_nvp("meanATWDCharge", meanATWDCharge_);
      ar & make_nvp("meanFADCCharge", meanFADCCharge_);
    }
  if (version == 11)
    {
      // these only existed in version 11, replaced with
      // meanATWDCharge_/meanFADCCharge_ == NaN in v12
      bool meanATWDChargeValid(false);
      bool meanFADCChargeValid(false);
      ar & make_nvp("meanATWDChargeValid", meanATWDChargeValid);
      ar & make_nvp("meanFADCChargeValid", meanFADCChargeValid);
      
      if (!meanATWDChargeValid) meanATWDCharge_=NAN;
      if (!meanFADCChargeValid) meanFADCCharge_=NAN;
    }
}

// now define some printing operators so we get better output
// than the address of the object

std::ostream& operator<<(std::ostream& oss, const LinearFit& f)
{
  oss << "[LinearFit slope(" << f.slope << ") intercept(" << f.intercept
      << ")]" ;
  return oss;
}

std::ostream& operator<<(std::ostream& oss, const QuadraticFit& f)
{
  oss << "[QuadraticFit a(" << f.quadFitA << ") b(" << f.quadFitB
      << ") c(" << f.quadFitC << ")]" ;
  return oss;
}

std::ostream& operator<<(std::ostream& oss, const SPEChargeDistribution& spe)
{

  oss << "[ SPEChargeDistribution :: " << std::endl
      << "               Exp1 Amp. : " << spe.exp1_amp << std::endl
      << "              Exp1 Width : " << spe.exp1_width << std::endl
      << "               Exp2 Amp. : " << spe.exp2_amp << std::endl
      << "              Exp2 Width : " << spe.exp2_width << std::endl
      << "           Gaussian Amp. : " << spe.gaus_amp << std::endl
      << "           Gaussian Mean : " << spe.gaus_mean << std::endl
      << "          Gaussian Width : " << spe.gaus_width << std::endl
      << "     Compensation Factor : " << spe.compensation_factor << std::endl
      << "       SLC Gaussian Mean : " << spe.SLC_gaus_mean << std::endl
      << "]" ;
  return oss;
}

std::ostream& operator<<(std::ostream& oss, const TauParam& p)
{
  oss << "[ TauParam :: " << std::endl
      << "         P0 : " << p.P0 << std::endl
      << "         P1 : " << p.P1 << std::endl
      << "         P2 : " << p.P2 << std::endl
      << "         P3 : " << p.P3 << std::endl
      << "         P4 : " << p.P4 << std::endl
      << "         P5 : " << p.P5 << std::endl
      << "    TauFrac : " << p.TauFrac << std::endl
      << "]" ;
  return oss;
}

std::ostream& operator<<(std::ostream& oss, const I3DOMCalibration& c)
{
  oss << "[ I3DOMCalibration  :: " << std::endl
      << "       TauParameters : " << c.GetTauParameters() << std::endl
      << "         Temperature : " << c.GetTemperature() << std::endl
      << "            FADCGain : " << c.GetFADCGain() << std::endl
      << "     FADCBaselineFit : " << c.GetFADCBaselineFit() << std::endl
      << "  FADCBeaconBaseline : " << c.GetFADCBeaconBaseline() << std::endl
      << "          FADCDeltaT : " << c.GetFADCDeltaT() << std::endl
      << "   FrontEndImpedance : " << c.GetFrontEndImpedance() << std::endl
      << "         ATWDGain[0] : " << c.GetATWDGain(0) << std::endl
      << "         ATWDGain[1] : " << c.GetATWDGain(1) << std::endl
      << "         ATWDGain[2] : " << c.GetATWDGain(2) << std::endl
      << "      ATWDFreqFit[0] : " << c.GetATWDFreqFit(0) << std::endl
      << "      ATWDFreqFit[1] : " << c.GetATWDFreqFit(1) << std::endl
      << "         TransitTime : " << c.GetTransitTime() << std::endl
      << "           HVGainFit : " << c.GetHVGainFit() << std::endl
      << "       DOMCalVersion : " << c.GetDOMCalVersion() << std::endl
      << "       ATWDDeltaT[0] : " << c.GetATWDDeltaT(0) << std::endl
      << "       ATWDDeltaT[1] : " << c.GetATWDDeltaT(1) << std::endl
      << "        SPEDiscCalib : " << c.GetSPEDiscCalib() << std::endl
      << "        MPEDiscCalib : " << c.GetMPEDiscCalib() << std::endl
      << "        PMTDiscCalib : " << c.GetPMTDiscCalib() << std::endl
      << "      RelativeDomEff : " << c.GetRelativeDomEff() << std::endl
      << "        DomNoiseRate : " << c.GetDomNoiseRate() << std::endl
      << " DomNoiseThermalRate : " << c.GetDomNoiseThermalRate() << std::endl
      << "   DomNoiseDecayRate : " << c.GetDomNoiseDecayRate() << std::endl
      << "DomNoiseScintillationMean : " << c.GetDomNoiseScintillationMean() << std::endl
      << "DomNoiseScintillationSigma : " << c.GetDomNoiseScintillationSigma() << std::endl
      << "DomNoiseScintillationHits : " << c.GetDomNoiseScintillationHits() << std::endl
      << "CombinedSPEChargeDistribution : " << c.GetCombinedSPEChargeDistribution() << std::endl
      << "      MeanATWDCharge : " << c.GetMeanATWDCharge() << std::endl
      << "      MeanFADCCharge : " << c.GetMeanFADCCharge() << std::endl
      << "]" ;
  return oss;
}

std::ostream& operator<<(std::ostream& oss, const I3DOMCalibrationMap& m)
{
  oss << "[ I3DOMCalibrationMap :: " << std::endl;
  I3DOMCalibrationMap::const_iterator iter = m.begin();
  for (;iter != m.end();iter++)
  {
    oss << "  " << iter->first << " : " << iter->second << std::endl;
  }
  oss << "]" ;
  return oss;
}

I3_SERIALIZABLE(I3DOMCalibration);
