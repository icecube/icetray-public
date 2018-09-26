/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> $LastChangedBy$
 */

#include "tableio/converter/I3WaveformConverter.h"
#include "dataclasses/I3DOMFunctions.h"
#include "dataclasses/physics/I3Waveform.h"
#include "dataclasses/calibration/I3Calibration.h"
#include "dataclasses/status/I3DetectorStatus.h"
#include "icetray/I3Units.h"
#include "icetray/I3Frame.h"

/******************************************************************************/

I3WaveformConverter::I3WaveformConverter(std::string atwdName, std::string fadcName, bool calibrate) :
    I3ConverterImplementation<I3WaveformSeriesMap>(),
    atwdName_(atwdName),
    fadcName_(fadcName),
    calibrate_(calibrate) 
{}

/******************************************************************************/

I3WaveformConverter::I3WaveformConverter() :
    I3ConverterImplementation<I3WaveformSeriesMap>() {
    log_fatal("%s: this constructor should not be callable", __PRETTY_FUNCTION__);
}

/******************************************************************************/

I3TableRowDescriptionPtr I3WaveformConverter::CreateDescription(const I3WaveformSeriesMap& waveforms) {
    I3TableRowDescriptionPtr desc = I3TableRowDescriptionPtr(new I3TableRowDescription() );
    std::string unit;
    std::string doc;
    if (calibrate_) {
        unit = "pe";
        doc = "calibrated waveform in units of photoelectrons";
    }
    else {
        unit = "mV";
        doc = "calibrated waveform in units of mV";
    }
    
    desc->isMultiRow_ = true;
    desc->AddField<int32_t>("string", "", "String number");
    desc->AddField<uint32_t>("om", "", "OM number");
    
    desc->AddField<bool>("atwd_ok", "bool", "status flag that atwd waveform has been convertered");
    desc->AddField<double>("atwd_t0", "ns", "start time of ATWD waveform");
    desc->AddField<double>("atwd_dt", "ns", "width of ATWD waveform bins");
    desc->AddField<double>("atwd", unit, doc, 128);
    desc->AddField<bool>("fadc_ok", "bool", "status flag that fadc waveform has been convertered");
    desc->AddField<double>("fadc_t0", "ns", "start time of FADC waveform");
    desc->AddField<double>("fadc_dt", "ns", "width of FADC waveform bins");
    desc->AddField<double>("fadc", unit, doc, 256);
    
    return desc;
}

/******************************************************************************/
// FIXME:
// drawback of the converter design: "rows" has the size given by GetNumberOfRows()
// but this function sees only one of the two waveform maps. Convention: attach
// the converter to the ATWD waveform map. If the FADC waveform map contains more
// entries thah the ATWD map, the excessing waveforms will be discarded!

size_t I3WaveformConverter::FillRows(const I3WaveformSeriesMap& atwdWaveformMap, 
                                     I3TableRowPtr rows) {
     
    // retrieve calibration and detector status information
    // bail-out if not available
    I3DetectorStatusConstPtr detectorstatus = 
        currentFrame_->Get<I3DetectorStatusConstPtr>();
	I3CalibrationConstPtr calibration =
        currentFrame_->Get<I3CalibrationConstPtr>();
    if (!detectorstatus) {
        log_fatal("%s: couldn't find detector status information in current frame!",
                  __PRETTY_FUNCTION__);
    }
    if (!calibration) {
        log_fatal("%s: couldn't find calibration information in current frame!",
                   __PRETTY_FUNCTION__);
        return 0;
    }
    

    // retrieve the fadc waveform and set the number_????_oms counter
    // to indicate missing fadc  
    
    I3WaveformSeriesMapConstPtr fadcWaveformMap = 
        currentFrame_->Get<I3WaveformSeriesMapConstPtr>(fadcName_);
    
    if (!fadcWaveformMap) {
        log_warn("%s: couldn't find FADC waveform with name %s in current frame!",
                 __PRETTY_FUNCTION__, fadcName_.c_str());
    }

  
    const std::map<OMKey,I3DOMCalibration> &domcalmap = calibration->domCal;
    const std::map<OMKey,I3DOMStatus> &domstatusmap   = detectorstatus->domStatus;

    I3Map<OMKey, std::vector<I3Waveform> >::const_iterator iter;
    
    const size_t startRow = rows->GetCurrentRow();
    
    size_t currentRow;
    for (iter = atwdWaveformMap.begin(), currentRow = rows->GetCurrentRow(); 
         iter != atwdWaveformMap.end(); ++iter, ++currentRow) {

        rows->SetCurrentRow(currentRow);

        OMKey key = iter->first;
        const I3DOMCalibration &domcal = domcalmap.find(key)->second;
        const I3DOMStatus &domstatus   = domstatusmap.find(key)->second;
        const double GI(SPEMean(domstatus,domcal)*domcal.GetFrontEndImpedance());
    
        if( std::isnan(GI) ){
            log_info("OM (%d,%d) has an invalid gain. Skipping the OM.", 
                     key.GetString(), key.GetOM());
        }
        
        // /!\ only the first atwd waveform is extracted
        const I3Waveform& wf = iter->second.front();
        const std::vector<double>& atwd_readout = wf.GetWaveform();
        rows->Set<double>("atwd_t0", wf.GetStartTime()/I3Units::ns);
        rows->Set<double>("atwd_dt", wf.GetBinWidth()/I3Units::ns);
        double VoltToNPE = wf.GetBinWidth()/GI;

        std::vector<double>::const_iterator wfiter;
        double* buffer = rows->GetPointer<double>("atwd");
        int i = 0;

        if (calibrate_) {
            for(wfiter = atwd_readout.begin(); 
                i < 128 && wfiter != atwd_readout.end(); 
                ++i, ++wfiter) {
                buffer[i] = (*wfiter)*VoltToNPE;
            }
        }
        else {
            for(wfiter = atwd_readout.begin(); 
                i < 128 && wfiter != atwd_readout.end(); 
                ++i, ++wfiter) {
                buffer[i] = *wfiter/I3Units::mV;
            }
        }
        buffer = 0;
    

        I3Map<OMKey, std::vector<I3Waveform> >::const_iterator fadc_iter;
        if ( fadcWaveformMap && 
             (fadc_iter = fadcWaveformMap->find(key)) != fadcWaveformMap->end() ) {

            // /!\ only the first atwd waveform is extracted
            const I3Waveform& wf2 = fadc_iter->second.front();
            const std::vector<double>& fadc_readout = wf2.GetWaveform();
            rows->Set<double>("fadc_t0", wf2.GetStartTime()/I3Units::ns);
            rows->Set<double>("fadc_dt", wf2.GetBinWidth()/I3Units::ns);
            VoltToNPE = wf2.GetBinWidth()/GI;

            std::vector<double>::const_iterator wfiter;
            buffer = rows->GetPointer<double>("fadc");
            i = 0;

            if (calibrate_) {
                for(wfiter = fadc_readout.begin(); 
                    i < 256 && wfiter != fadc_readout.end(); ++i, ++wfiter) {
                    buffer[i] = (*wfiter)*VoltToNPE;
                }
            }
            else {
                for(wfiter = fadc_readout.begin(); 
                    i < 256 && wfiter != fadc_readout.end(); ++i, ++wfiter) {
                    buffer[i] = *wfiter/I3Units::mV;
                }
            }
        }
    }
    return currentRow - startRow;

}

/******************************************************************************/

size_t I3WaveformConverter::GetNumberOfRows(const I3WaveformSeriesMap& waveforms) {
    return waveforms.size();
}

/******************************************************************************/
