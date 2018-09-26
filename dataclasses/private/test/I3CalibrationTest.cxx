
#include <I3Test.h>

#include "dataclasses/calibration/I3Calibration.h"
#include "icetray/OMKey.h"
#include "icetray/I3Units.h"
#include <dataclasses/Utility.h>
#include <string>
#include <iostream>

using std::string;
using std::cout;
using std::endl;

TEST_GROUP(I3DOMCalibrationTest);

// Test conversion back and forth from counts to volts
TEST(bin_parameters)
{	
    I3CalibrationPtr calib(new I3Calibration());
    OMKey omkey(20,20);
    
    I3DOMCalibration dom_calib;
    
    int id = 0;
    int channel = 0;
    int bin = 127;  //Set() method expects time reversed order.this is earliest bin
  
    double slope = -0.002*I3Units::V;  // volt/count
    
    const double temp = 900.00*I3Units::kelvin;
    
    double feImpedance = 45.2*I3Units::ohm;
    
    double gain = -17.0;

    dom_calib.SetTemperature(temp);

    dom_calib.SetFrontEndImpedance(feImpedance);
    
    dom_calib.SetATWDGain(channel,gain);
    
    dom_calib.SetATWDBinCalibSlope(id,
				  channel,
				  bin,
				  slope);

    LinearFit specal;
    // Taken from actual domcal file
    specal.slope = 0.0137063;
    specal.intercept = -7.39653;
    
    dom_calib.SetSPEDiscCalib(specal);

    calib->domCal[omkey] = dom_calib;

    I3VEMCalibration tank_calib;
    tank_calib.pePerVEM = 45.3;
    tank_calib.muPeakWidth = 15.2*I3Units::ns;

    calib->vemCal[omkey] = tank_calib;

    ENSURE_DISTANCE(gain, 
		    calib->domCal[omkey].GetATWDGain(channel), 
		    0.0001,
		    "Failed to get proper gain from I3DOMCalibration");
      
    bin = 0; //We store the domcal info in proper time order

    //check we got what we stored.
    ENSURE_DISTANCE(900.00,
		    calib->domCal[omkey].GetTemperature()/I3Units::kelvin,0.1,
		    "Temperature came back from storage with wrong value");

    ENSURE_DISTANCE(45.2,
		    calib->domCal[omkey].GetFrontEndImpedance()/I3Units::ohm,0.1,
		    "Temperature came back from storage with wrong value");

    ENSURE_DISTANCE(-0.002,
		    calib->domCal[omkey].GetATWDBinCalibSlope(id,channel,bin)/I3Units::V,
		    0.0001,
		    "Failed to properly return fit slope (test1)");
    
    ENSURE_DISTANCE(45.3,
		    calib->vemCal[omkey].pePerVEM,
		    0.0001,
	            "Failed to return proper I3VEMCalibration PEperVEM");
 
    ENSURE_DISTANCE(15.2,
		    calib->vemCal[omkey].muPeakWidth/I3Units::ns,
		    0.0001,
	            "Failed to return proper IVEMCalibration MuonWidth");

    ENSURE_DISTANCE(0.0137063,
		    calib->domCal[omkey].GetSPEDiscCalib().slope,
		    0.00001,
		    "Failed to return proper SPEDiscCalib slope");
    ENSURE_DISTANCE(-7.39653,
		    calib->domCal[omkey].GetSPEDiscCalib().intercept,
		    0.00001,
		    "Failed to return proper SPEDiscCalib intercept");

}

// Test I/O streams
TEST(to_stream)
{	
    I3DOMCalibrationPtr dom_calib(new I3DOMCalibration);
    
    int id = 0;
    int channel = 0;
    int bin = 1;

    double slope = -0.002*I3Units::V;
    double gain = -17.0;
	
    
    dom_calib->SetATWDGain(channel,gain);
    dom_calib->SetATWDBinCalibSlope(id,channel,bin,slope);
  
    log_info("Calibration %s",AsXML(*dom_calib).c_str());
}

TEST(simple_DomCalibration)
{
  I3DOMCalibration mydomcal;
  mydomcal.SetDomNoiseRate(650.0*I3Units::hertz);
  mydomcal.SetRelativeDomEff(1.25);
  ENSURE_DISTANCE(650.0,mydomcal.GetDomNoiseRate()/I3Units::hertz, 0.001,
		  "Noise rates do not match.");
  ENSURE_DISTANCE(1.25,mydomcal.GetRelativeDomEff(),0.001,
		  "RelativeDomEff do not match.");
}
