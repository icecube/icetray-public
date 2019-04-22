import os, numpy
from icecube import icetray, dataclasses, dataio
from I3Tray import *
from copy import deepcopy

def convert_omkey(key):
    try:
        string = int(key.split(',')[0])
        om = int(key.split(',')[1])
        omkey = icetray.OMKey(string, om)
        return omkey
    except ValueError:
        icetray.logging.log_warn("%s is not an OMKey" % str(key))

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

        cal = deepcopy(frame['I3Calibration'])
        #cal = frame['I3Calibration']
        del frame['I3Calibration']
        #domcal = cal.dom_cal
        attributes = ['exp1_amp','exp1_width', 'exp2_amp', 'exp2_width',
                      'gaus_amp', 'gaus_mean', 'gaus_width', 'compensation_factor',
                      'slc_gaus_mean']

        for key, fits in self.fit_values.items():
            omkey = convert_omkey(key)
            if not omkey:
                continue
            if int(key.split(',')[0]) < 87 and int(key.split(',')[1]) < 61:
                spe_distribution = dataclasses.SPEChargeDistribution()
                for attr in attributes:
                    if attr.startswith('slc_'):
                        setattr(spe_distribution, attr, fits['SLC_fit'][attr.replace('slc_','')])
                    else:
                        setattr(spe_distribution, attr, fits['ATWD_fit'][attr])

                if omkey in cal.dom_cal:
                    cal.dom_cal[omkey].combined_spe_charge_distribution = spe_distribution
                    if 'mean_atwd_charge' in fits['ATWD_fit']:
                        cal.dom_cal[omkey].mean_atwd_charge = fits['ATWD_fit']['mean_atwd_charge']
                    else:
                        print(fits['ATWD_fit'])
                    if 'mean_fadc_charge' in fits['SLC_fit']:
                        cal.dom_cal[omkey].mean_fadc_charge = fits['SLC_fit']['mean_fadc_charge']
                    else:
                        print(fits['SLC_fit'])
                else:
                    icetray.logging.log_warn("SPE Fit for %s has no calibration object." % str(omkey))
            del frame['I3Calibration']
            frame['I3Calibration'] = cal

    def __load_from_old_json(self, frame):
        from scipy.integrate import quad

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
