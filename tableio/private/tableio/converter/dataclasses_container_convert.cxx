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

#include <vector>
#include "dataclasses_container_convert.h"
#include <icetray/I3Logging.h>

/// @cond
namespace convert {

	void
	I3Trigger::AddFields(I3TableRowDescriptionPtr desc, const booked_type&)
	{
	        desc->AddField<double>("time", "ns", "time at which the trigger was issued");
	        desc->AddField<double>("length", "ns", "duration of triggered readout window");
	        desc->AddField<bool>("fired", "bool", "true, if trigger fired (used for simulations)");
    
	        // TriggerKey
	        MAKE_ENUM_VECTOR(sourceID,TriggerKey,TriggerKey::SourceID,TRIGGERKEY_H_TriggerKey_SourceID);
	        MAKE_ENUM_VECTOR(typeID,TriggerKey,TriggerKey::TypeID,TRIGGERKEY_H_TriggerKey_TypeID);
	        MAKE_ENUM_VECTOR(subtypeID,TriggerKey,TriggerKey::SubtypeID,TRIGGERKEY_H_TriggerKey_SubtypeID);
    
	        desc->AddEnumField<TriggerKey::SourceID>("source_id", sourceID, "",
	                                                 "Enumeration describing what 'subdetector' issued a trigger");
	        desc->AddEnumField<TriggerKey::TypeID>("type_id", typeID, "",
	                                               "Enumeration describing what 'algorithm' issued a trigger");
	        desc->AddEnumField<TriggerKey::SubtypeID>("subtype_id", subtypeID, "",
	                                                  "Enumeration describing how a software trigger was orginally 'configured' within the TWR DAQ trigger system");
	        desc->AddField<int32_t>("config_id", "", "Internal ID of the trigger settings in the DAQ. This can be used to retrieve the threshold, readout window, DOM set, etc from the DetectorStatus.");
	}
	
	void
	I3Trigger::FillSingleRow(const booked_type& trigger, I3TableRowPtr row)
	{
		row->Set<double>("time", trigger.GetTriggerTime());
		row->Set<double>("length", trigger.GetTriggerLength());
		row->Set<bool>("fired", trigger.GetTriggerFired());

		const TriggerKey& key = trigger.GetTriggerKey();

		row->Set<TriggerKey::SourceID> ("source_id",   key.GetSource());
		row->Set<TriggerKey::TypeID> ("type_id",       key.GetType());
		row->Set<TriggerKey::SubtypeID> ("subtype_id", key.GetSubtype());
		if (key.CheckConfigID())
			row->Set<int32_t>("config_id", key.GetConfigID());
		
	}

  void I3DOMLaunch::AddFields(I3TableRowDescriptionPtr desc, const booked_type&)
  {
    desc->AddField<double>("start_time","ns","Time at which the DOM went over threshold");
    desc->AddField<bool>("pedestal_sub","bool","Has the pedestal been subtracted from the waveform?");
    desc->AddField<bool>("lc_bit","bool","Was the local-coincidence condition satistfied?");
    
    MAKE_ENUM_VECTOR(atwdselect, ::I3DOMLaunch, ATWDselect, I3DOMLAUNCH_H_I3DOMLaunch_ATWDselect);
    desc->AddEnumField< ::I3DOMLaunch::ATWDselect >("which_atwd", atwdselect, "", "Which ATWD chip recorded this launch?");
    MAKE_ENUM_VECTOR(trigger_type, ::I3DOMLaunch, TriggerType, I3DOMLAUNCH_H_I3DOMLaunch_TriggerType);
    desc->AddEnumField< ::I3DOMLaunch::TriggerType>("trigger_type", trigger_type, "", "");     
    MAKE_ENUM_VECTOR(trigger_mode, ::I3DOMLaunch, TriggerMode, I3DOMLAUNCH_H_I3DOMLaunch_TriggerMode);
    desc->AddEnumField< ::I3DOMLaunch::TriggerMode>("trigger_mode", trigger_mode, "", "");
    
    desc->AddField<uint16_t>("raw_atwd_0", "counts", "Raw digitizer counts from ATWD channel 0", 128);
    desc->AddField<uint16_t>("raw_atwd_1", "counts", "Raw digitizer counts from ATWD channel 1", 128);
    desc->AddField<uint16_t>("raw_atwd_2", "counts", "Raw digitizer counts from ATWD channel 2", 128);
    desc->AddField<uint16_t>("raw_charge_stamp", "counts",
			     "The number of counts in highest-charge bin of the first 16 fADC bins (400 ns), plus the bins immediately before and after.",
			     3);
    desc->AddField<uint32_t>("raw_atwd_charge_stamp", "counts", "ATWD charge stamp used for IceTop style SLC readout");
    desc->AddField<uint8_t>("raw_atwd_charge_stamp_channel", "atwd_channel", "ATWD channel used for the ATWD charge stamp");
    desc->AddField<uint16_t>("raw_fadc", "counts", "Raw digitizer counts from fADC", 256);
  }

  void I3DOMLaunch::FillSingleRow(const booked_type& dl, I3TableRowPtr row) 
  {
    row->Set<double>("start_time", dl.GetStartTime());
    row->Set< ::I3DOMLaunch::ATWDselect>("which_atwd",dl.GetWhichATWD());
    row->Set< ::I3DOMLaunch::TriggerMode>("trigger_mode",dl.GetTriggerMode());
    row->Set< ::I3DOMLaunch::TriggerType>("trigger_type",dl.GetTriggerType());
    row->Set<bool>("pedestal_sub", dl.GetIsPedestalSub());
    row->Set<bool>("lc_bit", dl.GetLCBit());
    row->Set<uint32_t>("raw_atwd_charge_stamp", dl.GetRawATWDChargeStamp());
    row->Set<uint8_t>("raw_atwd_charge_stamp_channel", dl.GetWhichATWDChargeStamp());
    std::vector<int32_t>::const_iterator it;
    size_t i; uint16_t* pointy;
      
    pointy = row->GetPointer<uint16_t>("raw_atwd_0");
    for (it = dl.GetRawATWD(0).begin(), i = 0; it != dl.GetRawATWD(0).end(); it++, i++) {
      pointy[i] = *it;
    }
    pointy = row->GetPointer<uint16_t>("raw_atwd_1");
    for (it = dl.GetRawATWD(1).begin(), i = 0; it != dl.GetRawATWD(1).end(); it++, i++) {
      pointy[i] = *it;
    }
    pointy = row->GetPointer<uint16_t>("raw_atwd_2");
    for (it = dl.GetRawATWD(2).begin(), i = 0; it != dl.GetRawATWD(2).end(); it++, i++) {
      pointy[i] = *it;
    }
    pointy = row->GetPointer<uint16_t>("raw_charge_stamp");
    for (it = dl.GetRawChargeStamp().begin(), i = 0; it != dl.GetRawChargeStamp().end(); it++, i++) {
      pointy[i] = *it;
    }
    pointy = row->GetPointer<uint16_t>("raw_fadc");
    for (it = dl.GetRawFADC().begin(), i = 0; it != dl.GetRawFADC().end(); it++, i++) {
      pointy[i] = *it;
    } 
  }


  void I3RecoHit::AddFields(I3TableRowDescriptionPtr desc, const booked_type&)
  {
    desc->AddField<double>("time", "ns", "time");
    desc->AddField<int32_t>("id", "generic", "hit id");
  }

  void I3RecoHit::FillSingleRow(const booked_type& hit, I3TableRowPtr row)
  {
    row->Set<double>("time", hit.GetTime());
    row->Set<int32_t>("id", hit.GetID());
  }


  void I3MCHit::AddFields(I3TableRowDescriptionPtr desc, const booked_type&)
  {
    desc->AddField<double>("time", "ns", "time");
    desc->AddField<uint64_t>("weight", "PE", "The number of photoelectrons the hit represents.");
    desc->AddField<double>("cherenkov_distance", "m", "The distance to the nominal Cherenkov emission point");
    MAKE_ENUM_VECTOR(hit_source, ::I3MCHit, HitSource, I3MCHIT_H_I3MCHit_HitSource);
    desc->AddEnumField< ::I3MCHit::HitSource>("source",hit_source,"","");
    
    // skipped: hitID, particleMajorID, particleMinorID
    
  }

  void I3MCHit::FillSingleRow(const booked_type& hit, I3TableRowPtr row)
  {
    row->Set<double>("time", hit.GetTime());
    row->Set<uint64_t>("weight", hit.GetNPE());
    row->Set<double>("cherenkov_distance", hit.GetCherenkovDistance());
    row->Set< ::I3MCHit::HitSource>("source", hit.GetHitSource());
  }


  void I3RecoPulse::AddFields(I3TableRowDescriptionPtr desc, const booked_type&)
  {
    desc->AddField<double>("time", "ns", "Leading-edge time of the pulse");
    desc->AddField<double>("width", "ns", "Duration of the pulse");
    desc->AddField<double>("charge", "PE", "Integrated pulse charge");
  }

  void I3RecoPulse::FillSingleRow(const booked_type& pulse, I3TableRowPtr row)
  {
    row->Set<double>("time", pulse.GetTime());
    row->Set<double>("width", pulse.GetWidth());
    row->Set<double>("charge", pulse.GetCharge());
  }


  void double_pair::AddFields(I3TableRowDescriptionPtr desc, const booked_type&)
  {
    desc->AddField< double >("first", "", "Vector of double-double pair: first item");
    desc->AddField< double >("second", "", "Vector of double-double pair: second item");
  }
  
  void double_pair::FillSingleRow(const booked_type &item, I3TableRowPtr row)
  {
    row->Set< double >("first", item.first);
    row->Set< double >("second", item.second);
  }


  void I3FlasherInfo::AddFields(I3TableRowDescriptionPtr desc, const booked_type&)
  {
    desc->AddField<int32_t>("string", "", "String number of the flashing DOM");
    desc->AddField<uint32_t>("om", "", "OM number of the flashing DOM");
    desc->AddField<double>("flash_time", "ns", "Time (in nsec) in 25 nsec units, of the LED flash time.");
    desc->AddField<uint32_t>("mask", "bitmask", "Indicates which LEDs were flashing");
    desc->AddField<int32_t>("width", "", "Flasher width");
    desc->AddField<int32_t>("rate", "", "Flasher rate");
    desc->AddField<int32_t>("brightness", "", "Brightness level (1..127)");
    
    // Maybe move those to another converter:
    desc->AddField<double>("atwd_bin_size", "ns", "Bin width of the ATWD3");
    desc->AddField<int32_t>("raw_atwd3", "counts", "ATWD waveform of the LED current pulse", 128);  // <-- a vector!
  }

  void I3FlasherInfo::FillSingleRow(const booked_type &flasherinfo, I3TableRowPtr row)
  {
    row->Set<int32_t>("string", flasherinfo.GetFlashingOM().GetString());
    row->Set<uint32_t>("om", flasherinfo.GetFlashingOM().GetOM());
    row->Set<double>("flash_time", flasherinfo.GetFlashTime());
    row->Set<uint32_t>("mask", flasherinfo.GetMask());
    row->Set<int32_t>("width", flasherinfo.GetWidth());
    row->Set<int32_t>("rate", flasherinfo.GetRate());
    row->Set<int32_t>("brightness", flasherinfo.GetLEDBrightness());
    row->Set<double>("atwd_bin_size", flasherinfo.GetATWDBinSize());
    
    const std::vector<int> &waveform(flasherinfo.GetRawATWD3());
    size_t bins = std::min(waveform.size(), size_t(128));   // maybe paranoid, but safer
    int32_t *buffer = row->GetPointer<int32_t>("raw_atwd3");
    for (size_t i = 0; i < bins; ++i) {
      buffer[i] = waveform[i];
    }
    
  }


  void OMKey::AddFields(I3TableRowDescriptionPtr desc, const booked_type&)
  {
    desc->AddField<int32_t>("string", "", "String number");
    desc->AddField<uint32_t>("om", "", "OM number");
  }

  void OMKey::FillSingleRow(const booked_type &key, I3TableRowPtr row)
  {
    row->Set<int32_t>("string", key.GetString());
    row->Set<uint32_t>("om", key.GetOM());
  }


  void TankKey::AddFields(I3TableRowDescriptionPtr desc, const booked_type&)
  {
    desc->AddField<int32_t>("string", "", "String number");
    desc->AddField<char>("tank", "", "Tank ID (A or B)");
  }

  void TankKey::FillSingleRow(const booked_type &key, I3TableRowPtr row)
  {
    row->Set<int32_t>("string", key.string);
    switch (key.tank) {
    case ::TankKey::TankA:
      row->Set<char>("tank", 'A');
      break;
    case ::TankKey::TankB:
      row->Set<char>("tank", 'B');
      break;
    default: log_fatal("Encountered an IceTop tank that is neither 'A' nor 'B'!!!");
    }
  }

}
/// @endcond
