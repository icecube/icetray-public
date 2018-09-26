#!/usr/bin/env python

from I3Tray import *
from icecube import icetray, dataclasses, dataio
from os.path import expandvars
### open the GCD file you want to change 
filein = expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_IC80_DC6.54655.i3.gz")
oldgcd = dataio.I3File(filein)

### make a new I3File to write you new G,C, and/or D frames to
outfile = "./NEW_GeoCalibDetectorStatus_IC80_DC6.54655.i3"
newgcd = dataio.I3File(outfile, 'w')

### loop over the frames until you find the one with the geometry in it
while oldgcd.more():
    frame = oldgcd.pop_frame()
    if frame.Has("I3DetectorStatus") : 

        ### get the geometry from the frame
        i3ds = frame.Get("I3DetectorStatus")

        ## clear the the old triggers
        i3ds.trigger_status.clear()

        ## Setup the SMT 8 trigger
        smt_ts = dataclasses.I3TriggerStatus()
        smt_tk = dataclasses.TriggerKey(dataclasses.IN_ICE,
                                        dataclasses.SIMPLE_MULTIPLICITY,
                                        1006)

        smt_ts.trigger_name = 'SimpleMajorityTrigger'

        try:
            trigSet = smt_ts.trigger_settings
        except TypeError:
            print('Boost has borked itself')
            sys.exit( 0 )
        
        trigSet['threshold'] = "8"
        trigSet['timeWindow'] = "5000"
        
        inice_trc = dataclasses.I3TriggerStatus.I3TriggerReadoutConfig()
        inice_trc.readout_time_minus = 4000
        inice_trc.readout_time_plus = 6000
        inice_trc.readout_time_offset = 0
        
        ictop_trc = dataclasses.I3TriggerStatus.I3TriggerReadoutConfig()
        ictop_trc.readout_time_minus = 10000
        ictop_trc.readout_time_plus = 10000
        ictop_trc.readout_time_offset = 0
        
        readoutSet = smt_ts.readout_settings
        
        readoutSet[dataclasses.I3TriggerStatus.INICE] = inice_trc
        readoutSet[dataclasses.I3TriggerStatus.ICETOP] = ictop_trc
        
        i3ds.trigger_status[smt_tk] = smt_ts
        
        ## String Trigger (5/7)
        str_ts = dataclasses.I3TriggerStatus()
        str_tk = dataclasses.TriggerKey(dataclasses.IN_ICE,
                                        dataclasses.STRING,
                                        1007)
        
        str_ts.trigger_name = 'ClusterTrigger'
        trigSet2 = str_ts.trigger_settings
        
        trigSet2['coherenceLength'] = "7"
        trigSet2['domSet'] = "2"
        trigSet2['multiplicity'] = "5" 
        trigSet2['timeWindow'] = "1500"
        
        strinice_trc = dataclasses.I3TriggerStatus.I3TriggerReadoutConfig()
        strinice_trc.readout_time_minus = 4000
        strinice_trc.readout_time_plus = 6000
        strinice_trc.readout_time_offset = 0
        
        strictop_trc = dataclasses.I3TriggerStatus.I3TriggerReadoutConfig()
        strictop_trc.readout_time_minus = 10000
        strictop_trc.readout_time_plus = 10000
        strictop_trc.readout_time_offset = 0
        
        readoutSet2 = str_ts.readout_settings
        
        readoutSet2[dataclasses.I3TriggerStatus.INICE] = strinice_trc
        readoutSet2[dataclasses.I3TriggerStatus.ICETOP] = strictop_trc
        
        i3ds.trigger_status[str_tk] = str_ts


        ## print what we've got
        for trigKey,trigStat in i3ds.trigger_status:
            if trigKey.check_config_id():
                print('Key',trigKey.source, trigKey.type, trigKey.config_id)
            else:
                print('Key',trigKey.source, trigKey.type)
            print('TriggerStatus for:',trigStat.trigger_name)
            print('ReadoutConfig:')
            readoutset = trigStat.readout_settings
            for e in readoutset:
                print(e.key())
                trc = e.data()
                print('tminu:',trc.readout_time_minus,' tplus:', trc.readout_time_plus, ' toffset:', trc.readout_time_offset)
            print('TriggerConfigs:')
            for foo,bar in trigStat.trigger_settings:
                print(foo, '->', bar)




        ### remove the I3Geometry from the old frame
        del frame["I3DetectorStatus"]   
        ### put the new I3Geometry in the frame 
        frame["I3DetectorStatus"] = i3ds  
    newgcd.push(frame)   ### push the frame to our empty I3File

newgcd.close()   ##close it

import os
os.unlink(outfile)

