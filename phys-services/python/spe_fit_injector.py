import json
import bz2
import numpy

from copy import deepcopy

from icecube import icetray
from icecube import dataclasses

class I3SPEFitInjector(icetray.I3Module):
    ''' This module loads the structures in dataclasses using values
        in the JSON file provided by the calibration group.
        https://wiki.icecube.wisc.edu/index.php/SPE_recalibration
    '''
    def __init__(self, context):
        icetray.I3Module.__init__(self, context)

        self.AddOutBox("OutBox")
        self.AddParameter("Filename", "JSON (may bz2 compressed) file with SPE fit data", "")

    def Configure(self):
        ''' Give the filename, read and load the constants in this method.'''
        self.filename = self.GetParameter("Filename")

        if self.filename.endswith('.bz2'):
            json_fit_values = json.loads(bz2.BZ2File(self.filename).read())
        else:
            f = open(self.filename)
            json_fit_values = json.load(f)

        self.fit_dict = dict()
        for key, data in json_fit_values.items():

            # we don't really use the validity date in offline anymore
            if key == 'valid_date':
                continue
            if key == 'year':
                continue

            # if none of the data is valid it's OK to skip entries.
            if bool(data['JOINT_fit']['valid']) == False and \
              bool(data['ATWD_fit']['valid']) == False and \
              bool(data['FADC_fit']['valid']) == False :
                continue
                
            string = int(key.split(",")[0])
            om = int(key.split(",")[1])

            omkey = icetray.OMKey(string, om)

            # set atwd/fadc means to NaN, consistent with
            # the treatment in dataclasses, if they're invalid
            atwd_mean = float(data['ATWD_fit']['gaus_mean']) \
                if bool(data['ATWD_fit']['valid']) == True \
                else numpy.nan
                
            fadc_mean = float(data['FADC_fit']['gaus_mean']) \
                if bool(data['FADC_fit']['valid']) == True \
                else numpy.nan

            exp_amp = float(data['JOINT_fit']['exp_norm'])
            exp_width = float(data['JOINT_fit']['exp_scale']) 
            gaus_amp = float(data['JOINT_fit']['gaus_norm'])
            gaus_mean = float(data['JOINT_fit']['gaus_mean'])
            gaus_width = float(data['JOINT_fit']['gaus_stddev'])
                        
            self.fit_dict[omkey] = {'atwd_mean' : atwd_mean,
                                    'fadc_mean' : fadc_mean,
                                    'exp_amp': exp_amp,
                                    'exp_width': exp_width,
                                    'gaus_amp': gaus_amp,
                                    'gaus_mean': gaus_mean,
                                    'gaus_width': gaus_width
                                    }
        
    def Calibration(self, frame):

        # make a deepcopy so we don't change objects expected
        # to be const on the C++ side. 
        cal = deepcopy(frame['I3Calibration'])
        del frame['I3Calibration']

        domcal = cal.dom_cal
        for omkey, i3domcal in domcal.items():
            if omkey in self.fit_dict:
                i3domcal.mean_atwd_charge = self.fit_dict[omkey]['atwd_mean']
                i3domcal.mean_fadc_charge = self.fit_dict[omkey]['fadc_mean']

                spe_charge_dist = dataclasses.SPEChargeDistribution()
                spe_charge_dist.exp_amp = self.fit_dict[omkey]['exp_amp']
                spe_charge_dist.exp_width = self.fit_dict[omkey]['exp_width']
                spe_charge_dist.gaus_amp = self.fit_dict[omkey]['gaus_amp']
                spe_charge_dist.gaus_mean = self.fit_dict[omkey]['gaus_mean']
                spe_charge_dist.gaus_width = self.fit_dict[omkey]['gaus_width']

                i3domcal.combined_spe_charge_distribution = spe_charge_dist
                cal.dom_cal[omkey] = i3domcal

        frame['I3Calibration'] = cal
        self.PushFrame(frame)
