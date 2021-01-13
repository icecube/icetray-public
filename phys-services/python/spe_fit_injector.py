import os
import numpy
from icecube import icetray
from icecube import dataclasses
from icecube import dataio
from copy import deepcopy

def convert_omkey(key):
    try:
        string = int(key.split(',')[0])
        om = int(key.split(',')[1])
        omkey = icetray.OMKey(string, om)
        return omkey
    except ValueError:
        icetray.logging.log_warn("%s is not an OMKey" % str(key))


def omkey_str(omkey):
    try:
        return str(int(omkey[0]))+','+str(int(int(omkey[1])))
    except ValueError:
        icetray.logging.log_warn("%s is not an OMKey" % str(omkey))


def is_new_style(fit_values):
    '''
    Determine whether we're looking at an old JSON
    file or new JSON file.
    '''
    try:
        if 'exp2_amp' in fit_values['1,1']['ATWD_fit'].keys():
            print('The provided JSON file appears to be a newer version.')
            print(' - Newer versions include: ')
            print(' - A description of the individual charge distributions with two exponentials and a gaussian. After the charge shift.')
            print(' - A compensation factor such that the DOM efficiency does not need to be scaled.')
            print(' - A mean ATWD and FADC charge, corresponding to the charge shift performed in data.')
            return True
    except:
        pass

    for key, fits in fit_values.items():
        if 'exp2_amp' in fits:
            print('The provided JSON file appears to be a newer version.')
            print(' - Newer versions include: ')
            print(' - A description of the individual charge distributions with two exponentials and a gaussian. After the charge shift.')
            print(' - A compensation factor such that the DOM efficiency does not need to be scaled.')
            print(' - A mean ATWD and FADC charge, corresponding to the charge shift performed in data.')
            return True

    print('The provided JSON file appears to be a older version.')
    print(' - Older versions were used for Pass2 charge scaling: ')
    print(' - The shape of the charge distribution should come from TA0003.')
    print(' - The mean ATWD and FADC charge values are the shifts performed in data.')
    print(' - We will set the charge distribution to TA0003 and have the gaussian mean at 1.0.')
    return False

class SPEFitInjector:
    def __init__(self, filename):
        import json
        self.filename = filename
        if not os.path.exists(self.filename):
            icetray.logging.log_fatal("%s file does not exist." % self.filename)
        with open(self.filename) as f:
            self.fit_values = json.load(f)
        self.new_style = is_new_style(self.fit_values)

    def __repr__(self):
        return 'FitInjector(%s)' % self.filename

    def __load_from_new_json(self, frame):
        from scipy.integrate import quad
        import os.path 
        import numpy as np
        

        feature = {}
        for s in range(1,87): 
            for d in range(1,61): 
                dom = str(int(s))+','+str(int(d)) 
                feature[dom] = {}

        string,om,r,eff,avg = np.loadtxt(os.path.expandvars("$I3_SRC/phys-services/resources/eff.txt"),unpack = True)
        for i in range(len(string)): 
            if string[i] < 87 and om[i] < 61: 
                dom = str(int(string[i]))+','+str(int(om[i])) 
                # eff == 1 means HQE
                if int(eff[i]) == 1: 
                    feature[dom]['HQE'] = 1
                else: 
                    feature[dom]['HQE'] = 0



        cal = deepcopy(frame['I3Calibration'])
        del frame['I3Calibration']
        attributes = ['exp1_amp','exp1_width', 'exp2_amp', 'exp2_width',
                      'gaus_amp', 'gaus_mean', 'gaus_width', 'compensation_factor',
                      'slc_gaus_mean']

        #for key, fits in self.fit_values.items():
        #    omkey = convert_omkey(key)
        for omkey, i3domcal in cal.dom_cal.items():
            dom = omkey_str(omkey)
            SPE_distribution = dataclasses.SPEChargeDistribution()

            if not omkey:
                continue
            if not int(omkey[0]) in range(1,87) or not int(omkey[1]) in range(1,61):
                dom = 'Average'
            else:
                if float(feature[dom]['HQE'])==1.0: 
                    print(float(feature[dom]['HQE'])) 
                    i3domcal.relative_dom_eff = float(self.fit_values[dom]['RDE']) 
                    print('RDE: '+str(float(self.fit_values[dom]['RDE'])))

            SPE_distribution.exp1_amp       = self.fit_values[dom]['ATWD_fit']['exp1_amp']
            SPE_distribution.exp1_width     = self.fit_values[dom]['ATWD_fit']['exp1_width']
            SPE_distribution.exp2_amp       = self.fit_values[dom]['ATWD_fit']['exp2_amp']
            SPE_distribution.exp2_width     = self.fit_values[dom]['ATWD_fit']['exp2_width']
            SPE_distribution.gaus_amp       = self.fit_values[dom]['ATWD_fit']['gaus_amp']
            SPE_distribution.gaus_mean      = self.fit_values[dom]['ATWD_fit']['gaus_mean']
            SPE_distribution.gaus_width     = self.fit_values[dom]['ATWD_fit']['gaus_width']
            SPE_distribution.slc_gaus_mean  = self.fit_values[dom]['SLC_fit']['gaus_mean']

            SPE_distribution.compensation_factor = self.fit_values[dom]['ATWD_fit']['compensation_factor']
            i3domcal.combined_spe_charge_distribution = SPE_distribution

            if 'mean_atwd_charge' in self.fit_values[dom]: 
                i3domcal.mean_atwd_charge = self.fit_values[dom]['mean_atwd_charge']
            if 'mean_fadc_charge' in self.fit_values[dom]: 
                i3domcal.mean_fadc_charge = self.fit_values[dom]['mean_fadc_charge']

            cal.dom_cal[omkey] = i3domcal
 
        frame['I3Calibration'] = cal

    def __load_from_old_json(self, frame):

        cal = deepcopy(frame['I3Calibration'])
        del frame['I3Calibration']
        domcal = cal.dom_cal

        for key, fits in self.fit_values.items():
            omkey = convert_omkey(key)
            if not omkey:
                continue
            if int(key.split(',')[0]) < 87 and int(key.split(',')[1]) < 61:
                if omkey not in domcal:
                    continue
                # we don't really use the validity date in offline anymore
                if key == 'valid_date':
                    continue
                if key == 'year':
                    continue

                # if none of the data is valid it's OK to skip entries.
                if bool(fits['JOINT_fit']['valid']) == False and \
                    bool(fits['ATWD_fit']['valid']) == False and \
                    bool(fits['FADC_fit']['valid']) == False :
                        continue

                i3domcal = domcal[omkey]
                spe_charge_dist = dataclasses.SPEChargeDistribution()
                spe_charge_dist.exp1_amp    = 0.
                spe_charge_dist.exp1_width  = 1.
                spe_charge_dist.exp2_amp    = 0.590791#float(fits['JOINT_fit']['exp_norm'])
                spe_charge_dist.exp2_width  = 0.5057#float(fits['JOINT_fit']['exp_scale'])
                spe_charge_dist.gaus_amp    = 0.959663#float(fits['JOINT_fit']['gaus_norm'])
                spe_charge_dist.gaus_mean   = 1.0#float(fits['JOINT_fit']['gaus_mean'])
                spe_charge_dist.gaus_width  = 0.2916# float(fits['JOINT_fit']['gaus_stddev'])
                spe_charge_dist.slc_gaus_mean = 1.0#float(fits['FADC_fit']['gaus_mean'])
                #The numbers above describe the SPE charge distribution TA0003. This is the definition of a compensation factor of 1.0
                spe_charge_dist.compensation_factor = 1.00

                i3domcal.combined_spe_charge_distribution = spe_charge_dist
                atwd_mean = float(fits['ATWD_fit']['gaus_mean']) \
                    if bool(fits['ATWD_fit']['valid']) == True \
                    else numpy.nan

                fadc_mean = float(fits['FADC_fit']['gaus_mean']) \
                    if bool(fits['FADC_fit']['valid']) == True \
                    else numpy.nan
                i3domcal.mean_atwd_charge = atwd_mean
                i3domcal.mean_fadc_charge = fadc_mean

                cal.dom_cal[omkey] = i3domcal
        del frame['I3Calibration']
        frame['I3Calibration'] = cal

    def __call__(self, frame):
        if self.new_style:
            self.__load_from_new_json(frame)
        else:
            self.__load_from_old_json(frame)

class I3SPEFitInjector(icetray.I3Module):
    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        self.AddParameter("Filename", "Uncompressed JSON file with SPE fit data", "")
    def Configure(self):
        self.spe_fit_injector = SPEFitInjector(self.GetParameter("Filename"))
    def Calibration(self, frame):
        self.spe_fit_injector(frame)
        self.PushFrame(frame)
