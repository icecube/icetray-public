#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2025 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

# This is an example script to create a GCD file that includes the Upgrade Module's Calibration
# and Detector Status objects, filled with "reasonable" non-constructor values. Used for testing
# and not intended to be a complete Upgrade GCD builder!
#
# It requires an Input GCD that already includes an Upgrade Geometry and then tacks on the additional
# frame objects onto this original GCD and saves as a new Output.

import os
import argparse
from os.path import expandvars

from icecube import icetray, dataclasses, dataio
from icecube.icetray import I3Tray, I3Frame, I3Units, OMKey


# define default path variables and filenames
working_dir = expandvars("$PWD")
default_output = 'GeoCalibDetectorStatus_ICUpgrade.test.i3.bz2'
default_GCD = expandvars("$I3_TESTDATA/GCD/GeoCalibDetectorStatus_ICUpgrade.v58.mixed.cleaned.womhacked.V0.i3.zst")

# get input / output file names and locations from arguments
parser = argparse.ArgumentParser(description="Brute-forces a GCD with Upgrade C/D objects filled with non-constructor values")

parser.add_argument('-i', '--infile', required=False, type=str, default=default_GCD,
                    help='Original GCD (including path) to input into the script')

parser.add_argument('-o', '--outfile', required=False, type=str, default=os.path.join(working_dir, default_output),
                    help='Output GCD filename (including path)')

args = parser.parse_args()
orig_GCD = args.infile
output = args.outfile


########################################
#      DEgg Calibration Template
########################################

DEggCalTemplate = dataclasses.I3DEggCal()

DEggCalTemplate.temperature = (-40 + 273.0) * I3Units.kelvin # (-40 degrees Celsius)

# from R5912-100 PMT Datasheet
DEggCalTemplate.pmt_transit_time = 54 * I3Units.ns

# from https://wiki.icecube.wisc.edu/index.php/D-Egg_MC_Inputs#ADC_baseline.2Fnoise
DEggCalTemplate.adc_baseline_rms = 2.5 # [Counts]

# from DEgg Paper
DEggCalTemplate.pmt_transit_time_spread = 2.89 * I3Units.ns

# from DEgg FAT (DEgg2020-1-048_v1)
DEggCalTemplate.hv_gain_relation.slope = 6.25    # [ln(Gain) / ln(V)]
DEggCalTemplate.hv_gain_relation.intercept = -30 # [ln(Gain)]

# from xDOMCAL User Guide
DEggCalTemplate.dac_baseline_relation.slope = 4/15  # [Counts / DAC]
DEggCalTemplate.dac_baseline_relation.intercept = 0 # [Counts]

# from Stephany Elizabeth Vargas Piedra's Master Thesis
DEggCalTemplate.tau_params.p0 = 4.9  * I3Units.microsecond
DEggCalTemplate.tau_params.p1 = 26.7 * I3Units.microsecond
DEggCalTemplate.tau_params.p2 = (28.3 + 273.0) * I3Units.kelvin # (28.3 degrees Celsius)


########################################
#      DEgg Detector Status Template
########################################

DEggStatusTemplate = dataclasses.I3DEggStatus()

DEggStatusTemplate.enabled = True # this PMT is sending readouts
DEggStatusTemplate.trig_mode = dataclasses.I3DEggStatus.FIR # FIR trigger
DEggStatusTemplate.trig_threshold = 12 # [Counts] spe threshold (0.25 PE)
DEggStatusTemplate.fir_coefficients = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0]
DEggStatusTemplate.readout_mode = dataclasses.I3DEggStatus.VARIABLE_LENGTH
DEggStatusTemplate.pre_samples = 16
DEggStatusTemplate.post_samples = 16
DEggStatusTemplate.pmt_hv = 1600 * I3Units.volt # to get 1e7 gain
DEggStatusTemplate.baseline_dac = 30000 # [DAC] to get 8000 count baseline


########################################
#      mDOM Calibration Template
########################################

mDOMCalTemplate = dataclasses.I3mDOMCal()

mDOMCalTemplate.adc_baseline_rms = 2.5 # [Counts]

# from mDOM FAT (mDOM_D232_v1)
mDOMCalTemplate.pmt_transit_time = 44         * I3Units.ns
mDOMCalTemplate.pmt_transit_time_spread = 2.5 * I3Units.ns
mDOMCalTemplate.hv_gain_relation.slope = 6.85      # [ln(Gain) / ln(V)]
mDOMCalTemplate.hv_gain_relation.intercept = -14.5 # [ln(Gain)]

# from domcal
mDOMCalTemplate.adc_baseline_dac = 13300  # [DAC]
mDOMCalTemplate.adc_baseline_value = 3600 # [Counts]
mDOMCalTemplate.disc_dac = 23500          # [DAC]
mDOMCalTemplate.disc_threshold = 0.002 * I3Units.volt


########################################
#      mDOM Detector Status Template
########################################

mDOMStatusTemplate = dataclasses.I3mDOMStatus()

mDOMStatusTemplate.enabled = True # this PMT is sending readouts
mDOMStatusTemplate.trig_mode = dataclasses.I3mDOMStatus.Disc # Discriminator Trigger
mDOMStatusTemplate.adc_threshold = 10 # [Counts]
mDOMStatusTemplate.readout_mode = dataclasses.I3mDOMStatus.VARIABLE_LENGTH
mDOMStatusTemplate.pre_samples = 16
mDOMStatusTemplate.post_samples = 16
mDOMStatusTemplate.pmt_hv = 85 * I3Units.volt # to get 1e7 gain
mDOMStatusTemplate.adc_baseline_dac = 13300 # [DAC] to get 3600 count baseline
mDOMStatusTemplate.disc_dac = 23500 # [DAC] to get SPE threshold (0.2 PE)


########################################
#         PMT Calibration Template
########################################

PMTCalTemplate = dataclasses.I3PMTCal()

PMTCalTemplate.relative_pmt_eff = 1
PMTCalTemplate.noise_rate = 5.5e-7 * (1/I3Units.ns) # [ns^-1]

# SPE Charge Distribution fit parameters from the HamamatsuR15458_02PMT::defaultChargeDistribution() in:
# https://github.com/icecube/icetray/blob/main/DOMLauncher/private/DOMLauncher/PMT.cxx
PMTCalTemplate.spe_charge_dist.pdfs.append(dataclasses.SPEChargeDistribution.Exponential(amplitude = 6.89998311,
                                                                                         width = 5.90687611e-03))
PMTCalTemplate.spe_charge_dist.pdfs.append(dataclasses.SPEChargeDistribution.Exponential(amplitude = 4.72420342e-01,
                                                                                         width = 2.48130571e-01))
PMTCalTemplate.spe_charge_dist.pdfs.append(dataclasses.SPEChargeDistribution.Gaussian(amplitude = 8.08336816e-01,
                                                                                      mean = 1.,
                                                                                      sigma = 4.40735435e-01))
PMTCalTemplate.spe_charge_dist.compensation_factor = 1.0
                                           


########################################


# Open the original Upgrade GCD and obtain the Geometry and Calibration
gcdfile = dataio.I3File(orig_GCD)
gcdframe = gcdfile.pop_frame()
geo = dataclasses.I3Geometry.from_frame(gcdframe)
allomgeo = geo.omgeo
gcdframe = gcdfile.pop_frame()
cal = dataclasses.I3Calibration.from_frame(gcdframe)
all_dom_cal = cal.dom_cal


# obtain lists of IceCube Gen1, Upgrade DEgg, and Upgrade mDOM OMKeys from the original GCD
Gen1_omkeys = []
mDOM_omkeys = []
DEgg_omkeys = []
for omkey in allomgeo:
    omgeo = allomgeo[omkey]

    if omgeo.omtype == dataclasses.I3OMGeo.IceCube:
        Gen1_omkeys.append(omkey)

    elif omgeo.omtype == dataclasses.I3OMGeo.mDOM:
        mDOM_omkeys.append(omkey)

    elif omgeo.omtype == dataclasses.I3OMGeo.DEgg:
        DEgg_omkeys.append(omkey)


def AddUpgradeC(frame):

    # Add a DEggCalTemplate for each DEgg OMKey
    degg_cals = dataclasses.I3DEggCalMap()
    for omkey in DEgg_omkeys:
        degg_cals[omkey] = DEggCalTemplate
    frame['I3DEggCalibration'] = degg_cals

    # Add an mDOMCalTemplate for each mDOM OMKey
    mdom_cals = dataclasses.I3mDOMCalMap()
    for omkey in mDOM_omkeys:
        mdom_cals[omkey] = mDOMCalTemplate
    frame['I3mDOMCalibration'] = mdom_cals

    return True


def AddUpgradeD(frame):

    # Add a DEggStatusTemplate for each DEgg OMKey
    degg_status = dataclasses.I3DEggStatusMap()
    for omkey in DEgg_omkeys:
        degg_status[omkey] = DEggStatusTemplate
    frame['I3DEggStatus'] = degg_status

    # Add an mDOMStatusTemplate for each mDOM OMKey
    mdom_status = dataclasses.I3mDOMStatusMap()
    for omkey in mDOM_omkeys:
        mdom_status[omkey] = mDOMStatusTemplate
    frame['I3mDOMStatus'] = mdom_status

    return True

   
def AddPMTCal(frame):

    pmt_cals = dataclasses.I3PMTCalMap()
    for omkey in allomgeo:
        
        # Add a PMTCalTemplate for each DEgg or mDOM OMKey
        if (omkey in DEgg_omkeys) or (omkey in mDOM_omkeys):
            pmt_cals[omkey] = PMTCalTemplate
            
        # For existing IceCube PMTs, add the RDE, Noise Rate and SPE Charge Distribution parameters already in the GCD
        elif omkey in Gen1_omkeys:
            # not every DOM has a key in dom_cal, so skip these over
            if omkey not in all_dom_cal:
                continue
            
            this_dom_cal = all_dom_cal[omkey]
            thisPMTCal = dataclasses.I3PMTCal()
            thisPMTCal.relative_pmt_eff = this_dom_cal.relative_dom_eff
            thisPMTCal.noise_rate = this_dom_cal.dom_noise_rate
            thisPMTCal.spe_charge_dist = this_dom_cal.combined_spe_charge_distribution
            pmt_cals[omkey] = thisPMTCal
            
    frame['I3PMTCalibration'] = pmt_cals

    return True
            

# I3Tray that runs the AddPMTCal, AddUpgradeC, and AddUpgradeD modules
tray = I3Tray()
tray.Add('I3Reader', Filename=orig_GCD)
tray.Add(AddPMTCal, streams=[I3Frame.Calibration])
tray.Add(AddUpgradeC, streams=[I3Frame.Calibration])
tray.Add(AddUpgradeD, streams=[I3Frame.DetectorStatus])
tray.Add('Dump')
tray.AddModule('I3Writer', 'writer', filename=output,
               Streams=[I3Frame.Geometry, I3Frame.Calibration, I3Frame.DetectorStatus])
tray.Execute()
tray.Finish()