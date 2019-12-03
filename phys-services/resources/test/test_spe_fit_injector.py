#!/usr/bin/env python

# $Id: $
# $Revision: $
# $LastChangedDate: $
# $LastChangedBy: $

import argparse
from os.path import expandvars

json_fn = expandvars("$I3_TESTDATA/sim/final-spe-fits-pole-run2015.json") 

parser = argparse.ArgumentParser(description='Process some integers.') 
parser.add_argument('--json-file', 
	 	    dest = 'json_fn', 
	 	    default = json_fn,  
	 	    help='Name of JSON file containing the SPE fit values.') 
args = parser.parse_args() 

import sys

from numpy import isnan

from I3Tray import I3Tray
from icecube import icetray, dataclasses, dataio, phys_services
from icecube.phys_services.spe_fit_injector import SPEFitInjector 
from icecube.phys_services.spe_fit_injector import I3SPEFitInjector
from icecube.icetray.I3Test import ENSURE

class SPEFitTestModule(icetray.I3Module) :
    ''' This module simply tests that we get the correct
        number of valid entries from the json data file.
        More information can be found here :
        https://wiki.icecube.wisc.edu/index.php/SPE_recalibration
    '''
    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        
    def Calibration(self, frame) :
        domcal = frame['I3Calibration'].dom_cal
        n_valid = sum([1 for omkey, i3domcal in domcal.iteritems() \
            if i3domcal.combined_spe_charge_distribution.is_valid])
        print(n_valid)
        n_nan_atwd_charge = 0
        n_nan_fadc_charge = 0
        n_nan_exp2_amp = 0
        for omkey, i3domcal in domcal.iteritems() :
            if i3domcal.combined_spe_charge_distribution.is_valid :
                # it's not true in general, but for this test file
                # whenever the combined fits are valid, the mean
                # ATWD and FADC charges should be non-NaN as well
                if isnan(i3domcal.mean_atwd_charge) :
                    n_nan_atwd_charge += 1
                if isnan(i3domcal.mean_fadc_charge) :
                    n_nan_fadc_charge += 1
                if isnan(i3domcal.combined_spe_charge_distribution.exp2_amp) :
                    n_nan_exp2_amp += 1
            
            if omkey[0]==1 and omkey[1]==1:
                print('\n -- Printing i3DOMCal Structure for DOM(1,1) -- ')
                print(i3domcal)
                print('\n\nOf Note: \n\
                    - CombinedSPEChargeDistribution should have non-NaNs.\n\
                    - MeanATWDCharge should be roughly 0.95 - 1.05. \n\
                    - MeanFADCCharge should be roughly 0.90 - 1.15.')

        spe_fi = SPEFitInjector(args.json_fn) 
	
        if spe_fi.new_style:                         
            # There are 5085 valid entries out of 5085 in the file IC86.2016_923_NewWaveDeform.json 
            # Not sure about other JSON files out there, so 5k seems reasonable. 
            if n_valid < 5000: 
                print("Expected 5085 valid entries.") 
                print("Got N valid = %d" % n_valid) 
                print("FAIL") 
                sys.exit(1) # report back to the mothership 
        else: 
            ENSURE(n_nan_atwd_charge == 0, "All the ATWD mean charges should be non-NaN.")
            ENSURE(n_nan_fadc_charge == 5, "There should be 5 NaN FADC charges.")
            ENSURE(n_nan_exp2_amp == 0,"All parameters describing the SPE Charge distribution should be non-NaN.")

            # there are 5050 valid entries out of 5085 in the file 'final-spe-fits-pole-run2015.json'
            if n_valid != 5050 :
                print("Expected 5050 valid entries.")
                print("Got N valid = %d" % n_valid)
                print("FAIL")
                sys.exit(1) # report back to the mothership

        self.PushFrame(frame)

gcd_fn = expandvars("$I3_TESTDATA/GCD/GeoCalibDetectorStatus_2013.56429_V1.i3.gz") 

tray = I3Tray()
tray.Add("I3InfiniteSource", prefix = gcd_fn )
tray.Add(I3SPEFitInjector, filename = args.json_fn)
tray.Add(SPEFitTestModule)
tray.Execute(4)

