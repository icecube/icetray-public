// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include <I3Test.h>

#include "dataclasses/calibration/I3Calibration.h"
#include "dataclasses/calibration/I3IceTopSLCCalibration.h"
#include "icetray/OMKey.h"
#include "icetray/I3Units.h"
#include "icetray/I3Frame.h"
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

    // Create a hypothetical VEMCal for this DOM (even though it's not an IceTop DOM):
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

    // Check the VEMCal constants
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

TEST(icetop_SLCcals)
{
    // Create a hypothetical IceTopSLCCal for a hypothetical DOM
    I3IceTopSLCCalibration itslc_calib;
    itslc_calib.intercept_C0A0 = 0.01;
    itslc_calib.intercept_C0A1 = 0.11;
    itslc_calib.intercept_C0A2 = 0.21;
    itslc_calib.intercept_C1A0 = 0.02;
    itslc_calib.intercept_C1A1 = 0.12;
    itslc_calib.intercept_C1A2 = 0.22;
    itslc_calib.intercept_CunkA0 = 0.03;
    itslc_calib.intercept_CunkA1 = 0.13;
    itslc_calib.intercept_CunkA2 = 0.23;
    itslc_calib.slope_C0A0 = 1.01;
    itslc_calib.slope_C0A1 = 1.02;
    itslc_calib.slope_C0A2 = 1.03;
    itslc_calib.slope_C1A0 = 1.11;
    itslc_calib.slope_C1A1 = 1.12;
    itslc_calib.slope_C1A2 = 1.13;
    itslc_calib.slope_CunkA0 = 1.21;
    itslc_calib.slope_CunkA1 = 1.22;
    itslc_calib.slope_CunkA2 = 1.23;
    itslc_calib.A0_A1_crossover = 20.0;
    itslc_calib.A1_A2_crossover = 100.0;

    // Enter it into the map
    OMKey k(7, 61);
    I3IceTopSLCCalibrationMap calmap;
    calmap[k] = itslc_calib;

    // Put it in a Collection
    I3IceTopSLCCalibrationCollectionPtr happy_frame_object(new I3IceTopSLCCalibrationCollection());
    happy_frame_object->startTime = I3Time(0);
    happy_frame_object->endTime = I3Time(0);
    happy_frame_object->ITslcCal = calmap;

    // Make a frame, to make sure we can add it as a I3FrameObject
    I3FramePtr frame(new I3Frame(I3Frame::Physics));
    frame->Put("TheSLCCollection", happy_frame_object);

    // Make sure we get back out what we put in
    I3IceTopSLCCalibrationCollection calcontainer_fetched = frame->Get<I3IceTopSLCCalibrationCollection>("TheSLCCollection");
    I3IceTopSLCCalibrationMap calmap_fetched = calcontainer_fetched.ITslcCal;
    ENSURE(calmap_fetched.size()==1, "Your SLCCal map is the wrong length.");
    for (unsigned int ia=0; ia<=2; ia++) {  // Loop over ATWD channels
        for (int ic=-1; ic<=1; ic++) {  // Loop over Chips (note: -1 is "unknown chip")
            float expect_int =  0.1*ia + 0.01*(ic+1)+0.03*(ic<0);
            log_trace("C %d / A %d / expect_int %f", ic, ia, expect_int);
            // Intercept
            ENSURE_DISTANCE(expect_int,
			    calmap_fetched[k].GetIntercept(ic, ia),
			    0.0001,
			    "Failed to return proper I3IceTopSLCCalibration intercept");

            // Slope
            float expect_sl = 1.0 + 0.1*ic+0.3*(ic<0) + 0.01*(ia+1);
            log_trace("C %d / A %d / expect_sl %f", ic, ia, expect_sl);
            ENSURE_DISTANCE(expect_sl,
			    calmap_fetched[k].GetSlope(ic, ia),
			    0.0001,
			    "Failed to return proper I3IceTopSLCCalibration slope");
        }
    }
    // ...and Crossovers
    ENSURE_DISTANCE(20.0,
		    calmap_fetched[k].A0_A1_crossover,
		    0.0001,
		    "Failed to return proper I3IceTopSLCCalibration A0->A1 crossover");
    ENSURE_DISTANCE(100.0,
		    calmap_fetched[k].A1_A2_crossover,
		    0.0001,
		    "Failed to return proper I3IceTopSLCCalibration A1->A2 crossover");

}
