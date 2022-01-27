/**
 * copyright  (C) 2006
 * the icecube collaboration
 * @version $Id$
 * @file dataclasses/private/dataclasses/I3DOMFunctions.cxx
 * @date $Date$
 */


#include "dataclasses/I3DOMFunctions.h"
#include "icetray/I3Units.h"
#include <vector>
#include <string>

double ATWDSamplingRate ( unsigned int chip,
			  const I3DOMStatus& status, 
			  const I3DOMCalibration& calib)
{
   double rateCorrected=0; //sampling rate in MHz
   const QuadraticFit atwdQFit  = calib.GetATWDFreqFit(chip);
   double dacTrigBias;
   if(std::isnan(atwdQFit.quadFitC)) // Old style linear fit
     {
       log_trace("Linear fit from DOMCAL");
       double slope = atwdQFit.quadFitB;
       double intercept = atwdQFit.quadFitA;
       if(chip == 0) dacTrigBias =  status.dacTriggerBias0;
       else if(chip == 1) dacTrigBias =  status.dacTriggerBias1;
       else log_fatal ("No trigger bias for chip %ui\n",chip);

       rateCorrected = (slope * dacTrigBias + intercept)*20.;  //
       log_trace("calculated rate corrected %f MHz, for chip %d", rateCorrected, chip);

       if (rateCorrected <= 0){
         log_debug("ATWDSamplingRate < 0 = %f GHz for chip %d", rateCorrected, chip);
         log_debug("Using Linear fit from DOMCAL");
         log_debug("rate = 20*(slope * dacTrigBias + intercept)");
         log_debug("  slope = %f", atwdQFit.quadFitB);
         log_debug("  intercept = %f", atwdQFit.quadFitA);
         log_debug("  dacTrigBias = %f", dacTrigBias);
       }
     }
   else // if not linear fit
     {
       log_trace("Quadratic Fit from DOMcal");
       
       if (atwdQFit.quadFitC == 0.0) 
	 {
	   log_warn("Found a quadratic fit with C=0.0, are you sure this is is a quadratic fit??");
	 } 
       else {                   
	 double c0 = atwdQFit.quadFitA;
	 double c1 = atwdQFit.quadFitB;
	 double c2 = atwdQFit.quadFitC;
	 if(chip == 0) dacTrigBias =  status.dacTriggerBias0;
	 else if(chip == 1) dacTrigBias =  status.dacTriggerBias1;
	 else log_fatal ("No trigger bias for chip %ui\n",chip);
	 
	 // f(MHz) = c2*dac*dac + c1*dac + c0
	 rateCorrected = (c2 * dacTrigBias * dacTrigBias + c1 * dacTrigBias + c0);
	 log_trace("I3DOMFunctions: rate corrected %f MHz, for chip %d", rateCorrected, chip);

         if (rateCorrected <= 0){
           log_debug("ATWDSamplingRate < 0 = %f GHz for chip %d", rateCorrected, chip);
           log_debug("Using Quadratic fit from DOMCAL");
           log_debug("rate = c2 * dacTrigBias * dacTrigBias + c1 * dacTrigBias + c0");
           log_debug("  c2 = %f", c2);
           log_debug("  c1 = %f", c1);
           log_debug("  c0 = %f", c0);
           log_debug("  dacTrigBias = %f", dacTrigBias);
         }
       }                         
     }
   return rateCorrected / I3Units::microsecond;
}

double PMTGain (const I3DOMStatus& status , 
		const I3DOMCalibration& calib)
{
  double log_gain = 0.0;
  double thegain = 0.0;
  double currentVoltage=(status.pmtHV/I3Units::volt);
  const LinearFit hvgain = calib.GetHVGainFit();
  
  if (hvgain.slope==0&&hvgain.intercept==0) {
    log_error("slope and intercept = 0");
    return NAN;
  }

  if(currentVoltage >0.0)
    {
      log_gain = hvgain.slope*log10(currentVoltage) + hvgain.intercept;
      thegain = pow(10.0,log_gain);
      
      log_trace("LOOK: pmt gain %f",thegain);
    }
  else
    {
      log_warn("DOM voltage is zero.  No Gain possible");
      
    }
  return thegain;
}

double SPEMean (const I3DOMStatus& status , 
		const I3DOMCalibration& calib)
{
  double spemean = 0.0;

  //First use PMTGain() to get the gain
  double pmtgain = PMTGain(status,calib);
  if(pmtgain>0.0) {
    spemean = pmtgain *I3Units::eSI*I3Units::C; 
  }
  else {
    log_error("DOM gain is zero or NAN. Return SPE=NAN");
    spemean = NAN;
  }
  return spemean;
}

double MeanSPECharge (const I3DOMCalibration& calib)
{
  const SPEChargeDistribution& spe_charge_dist = calib.GetCombinedSPEChargeDistribution();
  double mean_atwd_charge = (calib.IsMeanATWDChargeValid() ? calib.GetMeanATWDCharge(): 1);
  if (spe_charge_dist.IsValid()) return spe_charge_dist.Mean()*spe_charge_dist.compensation_factor/mean_atwd_charge;
  return 0.86/mean_atwd_charge; // Fall back for TA0003 SPETemplates. Derived from simulation
                                // fit by Claudio Kopper. See Diplomarbeit from Eike Middell for
                                // details.
}

double FADCBaseline (const I3DOMStatus& status , 
		     const I3DOMCalibration& calib)
{
  const LinearFit fadcbase = calib.GetFADCBaselineFit();
  double dacFADCRef = status.dacFADCRef;
  
  double baseline = fadcbase.slope * dacFADCRef + fadcbase.intercept;

  return baseline;
}

double TransitTime(const I3DOMStatus& status,
		   const I3DOMCalibration& calib)
{
    const LinearFit transitTimeFit = calib.GetTransitTime();
    double pmtHV = status.pmtHV/I3Units::V;
 
    // The linear relation returns the tarnsit time in ns.
    // transit time [ns] = slope / sqrt( V [volts]) + intercept
   
    double transitTime = transitTimeFit.slope/sqrt(pmtHV) + transitTimeFit.intercept;

    return transitTime*I3Units::ns;    
}

double SPEDiscriminatorThreshold(const I3DOMStatus& status,
				 const I3DOMCalibration& calib, 
                                 double const spePeakVoltageChargeRatio)
{
  const LinearFit speCalibFit = calib.GetSPEDiscCalib();
  double speDAC = status.speThreshold;

  //  Now use the linear relation between DAC and SPE Discriminator threshold:
  // and convert it in voltage (another voodoo factor...)
  double discrimThresh = spePeakVoltageChargeRatio * (speCalibFit.slope * speDAC + speCalibFit.intercept);
  
  log_trace("speDAC: %f   disc thresh: %f mV",speDAC,discrimThresh);

  return discrimThresh*I3Units::mV;
  
}

double MPEDiscriminatorThreshold(const I3DOMStatus& status,
				 const I3DOMCalibration& calib,
                                 double const spePeakVoltageChargeRatio)
{
  const LinearFit mpeCalibFit = calib.GetMPEDiscCalib();
  double mpeDAC = status.mpeThreshold;

  //  Now use the linear relation between DAC and MPE Discriminator threshold:
  // and get it in voltage 
  double discrimThresh = spePeakVoltageChargeRatio * (mpeCalibFit.slope * mpeDAC + mpeCalibFit.intercept);


  log_trace("mpeDAC: %f   disc thresh: %f mV",mpeDAC,discrimThresh);
  return discrimThresh*I3Units::mV;
  
}

double OldspeThreshold(const I3DOMStatus& status)
{
  double fePedest = (5.0 * (static_cast<double>(status.fePedestal))/4096.);
  double speThresher = ((5.*status.speThreshold/1024.- fePedest)/
			(9.6*(1+2200./249.))*I3Units::volt);
  
  return speThresher;
}

double OldmpeThreshold(const I3DOMStatus& status)
{
  double fePedest = (5.0 * (static_cast<double>(status.fePedestal))/4096.);
  double mpeThresher = 10.*((5.*status.mpeThreshold/1024.- fePedest)/
			    (9.6*(1+2200./249.))*I3Units::volt);
  
  return mpeThresher;
}

/*
 * 5/1024 "is just the conversion between a 10-bit DAC value and a voltage" (Spencer Axani 2019)
 * "The factor 9.6 is given by the gain product of U57 and U59, and (1 + 2200/249) is from the resistor divider attached to the discriminator DAC output" (Jim Braun 2019)
 * https://docushare.icecube.wisc.edu/dsweb/Get/Document-6683/sheet13.pdf
 */

double SPEPMTThreshold(const I3DOMStatus& status,
		    const I3DOMCalibration& calib)
{

  //
  // This function will attempt to always return the "best" SPE PMT Treshold.
  //   1.  Will check the domcal record, and see if slope/intercept of GetPMTDiscCalib()
  //   2a.  If these values are non-NAN, then use new calibration calculation relation 
  //       with these values.
  //   2b. If these values are NAN, will return to the safety of the OldspeThreshold()
  //        calculation.
  //
  const LinearFit pmtCalibFit = calib.GetPMTDiscCalib();
  const double speDAC = status.speThreshold;
  double speThresher;
  double magic_number{0.93};

  if (std::isnan(pmtCalibFit.slope) || std::isnan(pmtCalibFit.intercept))
    {  // Use OldseThreshold
      //Value returned is already stored with proper units.
      speThresher = OldspeThreshold(status);
      speThresher *= magic_number;
      log_trace("PMTDiscCalib was nan, using old method, found %f",
		speThresher);
    }
  else
    {  //New calc
      // slope of threshold vs DAC setting is given by constant factor (5/1024)*(9.6*(1+2200./249.))
      //     (this is the same as in OldspeThreshold())
      // but we use pmtCalibFit to tell us the DAC setting (dacOffset) which gives threshold=0
      //     (pmtCalibFit constants are measured in terms of charge not volts, but
      //      zero charge equals zero volts so dacOffset is still valid)
      //  Function returns in I3Units::Volts
      
      double dacOffset =  - pmtCalibFit.intercept / pmtCalibFit.slope ;
      speThresher = (5.*(speDAC-dacOffset)/1024.)/
	           (9.6*(1+2200./249.))*I3Units::V;

      // This .93 magic number will scale the discrimintor down to 0.2325PE.
      // Magic number needs to be replaced by a proper
      // handling of the SPEPMTThreshold
      speThresher *= magic_number;
      
      log_trace("PMTDiscCalib found, using best method, speDAC: %f   disc thresh: %f mV",
		speDAC,speThresher);
    }
  
  return speThresher;  
} 

std::vector<int> DOMCalVersion(const I3DOMCalibration& calib)
{
    // We assume here that the version given is sensible
    // i.e. something like "6.1.0"
    // Since it's a std::string I'm not sure how to deal
    // with whatever madness may be contained in it.

    std::string version = calib.GetDOMCalVersion();
    
    std::vector<int> nums;
    std::string::iterator iter;
    std::string num;
    
    for ( iter  = version.begin();
	  iter != version.end();
	  ++iter )
    {
	num.push_back(*iter);
	
	if ( *iter == '.' )
	{
	    num = num.erase(num.size() - 1);
	    nums.push_back(atoi(num.c_str()));
	    num = "";
	}
    }
    
    if ( num != "" )
	nums.push_back(atoi(num.c_str()));

    return nums;
}

unsigned int WhichATWD(const I3DOMLaunch& domLaunch)
{
    I3DOMLaunch::ATWDselect whichATWD = domLaunch.GetWhichATWD();
    
    if ( whichATWD == I3DOMLaunch::ATWDa )
        return 0;
    else if ( whichATWD == I3DOMLaunch::ATWDb )
        return 1;
    else 
        log_fatal("invalid ATWD ID in WhichATWD");
}
