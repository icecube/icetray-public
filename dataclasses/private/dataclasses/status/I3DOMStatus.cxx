#include <icetray/serialization.h>
#include <dataclasses/status/I3DOMStatus.h>
#include "icetray/I3Units.h"

I3DOMStatus::~I3DOMStatus() {}

template <class Archive>
void I3DOMStatus::serialize (Archive& ar, const unsigned version)
{
  if (version>i3domstatus_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3DOMStatus class.",version,i3domstatus_version_);

  ar & make_nvp("TrigMode",trigMode);
  ar & make_nvp("LCMode",lcMode);
  ar & make_nvp("LCWindowPre",lcWindowPre);
  ar & make_nvp("LCWindowPost",lcWindowPost);
  ar & make_nvp("LCSpan",lcSpan);
  ar & make_nvp("statusATWDa",statusATWDa);
  ar & make_nvp("statusATWDb",statusATWDb);
  ar & make_nvp("statusFADC",statusFADC);
  ar & make_nvp("PMTHV",pmtHV);
  ar & make_nvp("speThreshold",speThreshold);
  ar & make_nvp("fePedestal",fePedestal);       
  ar & make_nvp("dacTriggerBias0",dacTriggerBias0);
  ar & make_nvp("dacTriggerBias1",dacTriggerBias1);
  ar & make_nvp("dacFADCRef",dacFADCRef);
  ar & make_nvp("nBinsATWD0",nBinsATWD0);
  ar & make_nvp("nBinsATWD1",nBinsATWD1);
  ar & make_nvp("nBinsATWD2",nBinsATWD2);
  ar & make_nvp("nBinsATWD3",nBinsATWD3);
  ar & make_nvp("nBinsFADC",nBinsFADC);
  ar & make_nvp("fbState",fbState);
  ar & make_nvp("fbBrightness",fbBrightness);
  ar & make_nvp("fbLength",fbLength);
  ar & make_nvp("fbDelay",fbDelay);
  ar & make_nvp("fbMask",fbMask);
  ar & make_nvp("fbRate",fbRate);

  if(version > 0)
  {
      ar & make_nvp("domGainType",domGainType);
  }
  else
  {
      domGainType = I3DOMStatus::UnknownGainType;
  }	

  if (version > 0)
  {
      ar & make_nvp("cableType", cableType);
  }
  else 
  {
      cableType = I3DOMStatus::UnknownCableType;
  }
  if(version > 1)
  {
      ar & make_nvp("deltaCompress",deltaCompress);
  }
  else
  {
      deltaCompress = I3DOMStatus::Unknown;
  }	
  if(version > 2)
  {
      ar & make_nvp("SLCActive",SLCActive);
  }
  else
  {
      SLCActive = false;
  }	
  if(version > 3)
  {
    ar & make_nvp("mpeThreshold",mpeThreshold);
  }
  if(version < 5)
    // Correct old "psudo calibration" of DAC values back to raw DAC value from 
    //   older i3 files.  Newer files have DAC values from the DB correctly set.
    {
      double temp_feped = fePedestal/I3Units::volt;
      double temp_speThresh = speThreshold/I3Units::V;
      double temp_mpeThresh = mpeThreshold/I3Units::V;
      
      //Undo the voodoo, first for spe and mpe thresholds
      speThreshold = (1024./5.) * ( temp_speThresh*(9.6*(1+2200./249.))  
				    + temp_feped );
      mpeThreshold = (1024./5.) * ( (temp_mpeThresh/10.)*(9.6*(1+2200./249.))  
				    + temp_feped ); 
      fePedestal = 4096.0 * temp_feped / 5.0;
    }
  if(version > 5)
    {
      ar & make_nvp("TXMode",txMode);
    }

}

I3_SERIALIZABLE(I3DOMStatus);
