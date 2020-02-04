#include <I3Test.h>



#include "dataclasses/status/I3DOMStatus.h"
#include "dataclasses/calibration/I3DOMCalibration.h"
#include "dataclasses/I3DOMFunctions.h"
#include "icetray/I3Units.h"

#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

// Test some of Calibrate Module functionality
TEST_GROUP(DOMFunctions);

TEST(DoCalibTestDOMCalVersion)
{
    I3DOMCalibration calib;
    calib.SetDOMCalVersion("6.12.0");
    
    vector<int> version = DOMCalVersion(calib);
    ENSURE(version[0] == 6,  "Failed to get proper DOMCal version[0]");
    ENSURE(version[1] == 12, "Failed to get proper DOMCal version[1]");
    ENSURE(version[2] == 0,  "Failed to get proper DOMCal version[2]");
}

TEST(DoCalibTestLinearFit)
    {
      I3DOMStatus rawstatus;

      I3DOMCalibration calib;
    
      //Set some values
      rawstatus.pmtHV = 1400*I3Units::V;
      rawstatus.dacTriggerBias0 = 850;
      rawstatus.dacTriggerBias1 = 850;
      QuadraticFit qfit0,qfit1;
      //Values from an actual DOMCAL file.
      qfit0.quadFitA = 2.5858788;
      qfit0.quadFitB = 0.013337472;
      qfit0.quadFitC = NAN;  //Linear fit here
      qfit1.quadFitA = 2.3853257;
      qfit1.quadFitB = 0.014224272;
      qfit1.quadFitC = NAN;  //Linear fit here

      calib.SetATWDFreqFit(0,qfit0);
      calib.SetATWDFreqFit(1,qfit1);

      LinearFit hvgainfit;
      hvgainfit.intercept = -15.1997;
      hvgainfit.slope = 7.0842533;
      
      calib.SetHVGainFit(hvgainfit);
      
      double atwdrate0 =  ATWDSamplingRate(0,rawstatus,calib);
      double atwdrate1 =  ATWDSamplingRate(1,rawstatus,calib);
      // if we're going to get the gain, you oughta check it, but oh well.
      PMTGain(rawstatus,calib);
      double spemean = SPEMean(rawstatus,calib);

      //Compare the return values to values calculated by hand.
      
      ENSURE_DISTANCE(spemean/I3Units::pC,1.9632,
		      0.001,"Failed to return the proper SPEMean value");
      ENSURE_DISTANCE(atwdrate0*I3Units::microsecond, 278.455,
		      0.001,"Failed to return proper ATWD-A sampling rate");
      ENSURE_DISTANCE(atwdrate1*I3Units::microsecond,289.519,
		      0.001,"Failed to return proper ATWD-B sampling rate");
    }

TEST(DoCalibTestQuadraticFit)
    {
      I3DOMStatus rawstatus;

      I3DOMCalibration calib;
    
      //Set some values
      rawstatus.pmtHV = 1400*I3Units::V;
      rawstatus.dacTriggerBias0 = 850;
      rawstatus.dacTriggerBias1 = 850;
      QuadraticFit qfit0,qfit1;
      //Values from an actual DOMCAL file.
      qfit0.quadFitA = 24.725868;
      qfit0.quadFitB = 0.31952357;
      qfit0.quadFitC = -2.9083156E-5;  //quadratic fit here
      qfit1.quadFitA = 29.508028;
      qfit1.quadFitB = 0.31668407;
      qfit1.quadFitC = -2.9092209E-5;  //quadratic fit here

      calib.SetATWDFreqFit(0,qfit0);
      calib.SetATWDFreqFit(1,qfit1);

      LinearFit hvgainfit;
      hvgainfit.intercept = -15.1997;
      hvgainfit.slope = 7.0842533;
      
      calib.SetHVGainFit(hvgainfit);
      
      double atwdrate0 =  ATWDSamplingRate(0,rawstatus,calib);
      double atwdrate1 =  ATWDSamplingRate(1,rawstatus,calib);
      // if we're going to get the gain, you oughta check it, but oh well.
      PMTGain(rawstatus,calib);
      double spemean = SPEMean(rawstatus,calib);
      double pmtgain = PMTGain(rawstatus,calib);

      //Compare the return values to values calculated by hand.
      
      ENSURE_DISTANCE(spemean/I3Units::pC,1.9632,
		      0.001,"Failed to return the proper SPEMean value");
      ENSURE_DISTANCE(atwdrate0*I3Units::microsecond, 275.308,
		      0.001,"Failed to return proper ATWD-A sampling rate");
      ENSURE_DISTANCE(atwdrate1*I3Units::microsecond, 277.670,
		      0.001,"Failed to return proper ATWD-B sampling rate");
      ENSURE_DISTANCE(pmtgain, 1.2253719e7,
		      1.0e2,"Failed to return proper gain");
    }

TEST(DiscrimThresholds)
    {
      I3DOMStatus rawstatus;
      
      I3DOMCalibration calib;

      //Values taken from domcal file and GCD file...
      LinearFit speCal;
      speCal.slope = 0.0137063;
      speCal.intercept = -7.39653;
      calib.SetSPEDiscCalib(speCal);
      LinearFit mpeCal;
      mpeCal.slope = 0.131823;
      mpeCal.intercept = -70.4468;
      calib.SetMPEDiscCalib(mpeCal);

      rawstatus.fePedestal = 2130.;
      rawstatus.speThreshold = 620.;
      rawstatus.mpeThreshold = 560;

      double speDiscThresh = SPEDiscriminatorThreshold(rawstatus, calib);
      double mpeDiscThresh = MPEDiscriminatorThreshold(rawstatus, calib);

      ENSURE_DISTANCE(8.1*1.101376,
		      speDiscThresh/I3Units::mV, 0.00001,
		      "Failed to return proper calibrated SPE discriminator threshold");
      ENSURE_DISTANCE(8.1*3.374080,
		      mpeDiscThresh/I3Units::mV, 0.00001,
		      "Failed to return proper calibrated MPE discriminator threshold");

      speDiscThresh = SPEDiscriminatorThreshold(rawstatus, calib, 4);
      mpeDiscThresh = MPEDiscriminatorThreshold(rawstatus, calib, 4);

      ENSURE_DISTANCE(4*1.101376,
                      speDiscThresh/I3Units::mV, 0.00001,
                      "Failed to return proper calibrated SPE discriminator threshold");
      ENSURE_DISTANCE(4*3.374080,
                      mpeDiscThresh/I3Units::mV, 0.00001,
                      "Failed to return proper calibrated MPE discriminator threshold");

      double oldSPEDisc = OldspeThreshold(rawstatus);
      double oldMPEDisc = OldmpeThreshold(rawstatus);
      ENSURE_DISTANCE(4.5249879773951117E-12/I3Units::mV,
		      oldSPEDisc/I3Units::mV, 0.00001,
		      "OldSPEDisc failed to return old and busted values");
      ENSURE_DISTANCE(1.4221390786098917e-11/I3Units::mV,
		      oldMPEDisc/I3Units::mV, 0.00001,
		      "OldMPEDisc failed to return old and busted values");

    }

TEST(PMTDiscThresholdOldValue)
    {
      //This test will find nan for new PMT discrim calib.

      I3DOMStatus rawstatus;
      
      I3DOMCalibration calib;

      //Values taken from domcal file and GCD file...
      LinearFit speCal;
      speCal.slope = 0.0140356;
      speCal.intercept = -7.48874;
      calib.SetSPEDiscCalib(speCal);
      LinearFit pmtCal;
      pmtCal.slope = NAN;
      pmtCal.intercept = NAN;
      calib.SetPMTDiscCalib(pmtCal);

      rawstatus.fePedestal = 2130.;
      rawstatus.speThreshold = 620.;
      rawstatus.mpeThreshold = 560;

      double spePmtThresh = SPEPMTThreshold(rawstatus, calib);

      ENSURE_DISTANCE(4.524988*0.93,
		      spePmtThresh/I3Units::mV, 0.00001,
		      "Failed to return proper calibrated SPE PMT discriminator threshold");

    }

TEST(PMTDiscThresholdNewValue)
    {
      //This test will find correct values for new PMT discrim calib.

      I3DOMStatus rawstatus;
      
      I3DOMCalibration calib;

      //Values taken from domcal file and GCD file...
      LinearFit speCal;
      speCal.slope = 0.0140356;
      speCal.intercept = -7.48874;
      calib.SetSPEDiscCalib(speCal);
      LinearFit pmtCal;
      pmtCal.slope = 0.0166609;
      pmtCal.intercept = -8.78959;
      calib.SetPMTDiscCalib(pmtCal);

      rawstatus.fePedestal = 2130.;
      rawstatus.speThreshold = 620.;
      rawstatus.mpeThreshold = 560;

      double spePmtThresh = SPEPMTThreshold(rawstatus, calib);

      ENSURE_DISTANCE(spePmtThresh/I3Units::mV, 
		      4.44592,
		      0.00001,		      
		      "Failed to return proper calibrated SPE PMT discriminator threshold");

    }
