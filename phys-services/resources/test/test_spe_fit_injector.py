#!/usr/bin/env python3

import argparse
import sys
from os.path import expandvars

from numpy import isnan

from icecube import icetray
from icecube.icetray import I3Tray
from icecube import icetray, dataclasses, dataio, phys_services
from icecube.BadDomList.BadDomListTraySegment import BadDomList
from icecube.phys_services.spe_fit_injector import SPEFitInjector 
from icecube.phys_services.spe_fit_injector import I3SPEFitInjector
from icecube.offline_filterscripts.icetop_GCDmodification.overwrite_snowheights import ChangeSnowHeights_FromDB

json_fn = expandvars("$I3_TESTDATA/sim/final-spe-fits-pole-run2015.json")

parser = argparse.ArgumentParser(description='Test of I3SPEFitInjector. Injects the SPE fit parameters from a JSON file into the Calibration frame.\
Options include input and output GCD file, JSON file')
parser.add_argument('--json-file',
                    dest = 'json_fn',
                    default = json_fn,
                    help='Name of JSON file containing the SPE fit values.')
parser.add_argument("-i", "--input", action="store",
                    type=str, default="", dest="infile",
                    help="Input GCD i3 file")
parser.add_argument("-o", "--output", action="store",
                    type=str, default="", dest="outfile",
                    help="Output GCD i3 file")
parser.add_argument("--keep-gcd-atwd", action="store_true", default=False, dest="keepgcdatwd",
                    help="Keep mean_atwd_charge from GCD file, i.e. do not overwrite with value from JSON file (only for new style json) [default False]")
parser.add_argument("--keep-gcd-fadc", action="store_true", default=False, dest="keepgcdfadc",
                    help="Keep mean_fadc_charge from GCD file, i.e. do not overwrite with value from JSON file (only for new style json) [default False]")
parser.add_argument("--bad-dom-list", action="store_true", default=False, dest="baddomlist",
                    help="Add BadDomList tray segment")
parser.add_argument("--update-snow-heights", action="store_true", default=False, dest="updatesnow",
                    help="Add ChangeSnowHeights_FromDB module with localtesting option")
parser.add_argument("-r", "--run", action="store",
                    type=int, default=None, dest="runId",
                    help="Run number of GCD file, needed with options --bad-dom-list or --update-snow-heights")
args = parser.parse_args()

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
            # Reduce to 4987 for IC79.
            if n_valid < 4987:
                print("Expected 5085 valid entries.")
                print("Got N valid = %d" % n_valid)
                print("FAIL")
                sys.exit(1) # report back to the mothership
        else:
            assert n_nan_atwd_charge == 0, "All the ATWD mean charges should be non-NaN."
            assert n_nan_fadc_charge == 5, "There should be 5 NaN FADC charges."
            assert n_nan_exp2_amp == 0, "All parameters describing the SPE Charge distribution should be non-NaN."

            # there are 5050 valid entries out of 5085 in the file 'final-spe-fits-pole-run2015.json'
            if n_valid != 5050 :
                print("Expected 5050 valid entries.")
                print("Got N valid = %d" % n_valid)
                print("FAIL")
                sys.exit(1) # report back to the mothership

        self.PushFrame(frame)

gcd_fn = expandvars("$I3_TESTDATA/GCD/GeoCalibDetectorStatus_2013.56429_V1.i3.gz")
if args.infile:
    gcd_fn = args.infile
print('gcd_fn',gcd_fn)
print('json_fn',args.json_fn)

tray = I3Tray()
tray.Add("I3InfiniteSource", prefix = gcd_fn )
if args.baddomlist:
    if args.runId:
        tray.AddSegment(BadDomList, 'BadDomList', RunId = args.runId)
    else:
        print("Exiting because option --run is required with --bad-dom-list")
        sys.exit(1)
tray.Add(I3SPEFitInjector, filename = args.json_fn, KeepGcdATWD = args.keepgcdatwd, KeepGcdFADC = args.keepgcdfadc)
tray.Add(SPEFitTestModule)
if args.updatesnow:
    if args.runId:
        tray.Add(ChangeSnowHeights_FromDB, 'updateSnowHeights', Run = args.runId, localtesting = True)
    else:
        print("Exiting because option --run is required with --update-snow-heights")
        sys.exit(1)
# write to i3 file
if args.outfile:
    print('outfile',args.outfile)
    tray.AddModule("I3Writer", filename=args.outfile,
                   Streams=[icetray.I3Frame.TrayInfo, icetray.I3Frame.Geometry, icetray.I3Frame.Calibration, icetray.I3Frame.DetectorStatus],
               )

tray.Execute(4)

# print more CPU usage info than specified by default
tray.PrintUsage(fraction=1.0) 
