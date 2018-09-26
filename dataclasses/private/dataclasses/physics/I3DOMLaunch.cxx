#include <vector>
#include <icetray/serialization.h>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <dataclasses/physics/DeltaCompressor.h>
#include <stdexcept>
#include <dataclasses/I3Vector.h>
#include <boost/foreach.hpp>

I3DOMLaunch::I3DOMLaunch() 
  : startTime_(0.0),
    trigger_(TEST_PATTERN),
    mode_(UNDEFINED),
    whichATWD_(UNKNOWN_CHIP),
    rawATWDChargeStamp_(0),
    whichATWDChargeStamp_(0),
    localCoincidence_(false),
    pedestal_(false)
{
  rawATWD_.resize(4);
}

I3DOMLaunch::~I3DOMLaunch() {}


template <class Archive>
void I3DOMLaunch::save(Archive& ar, unsigned version) const
{
  ar & make_nvp("StartTime", startTime_);

  ar & make_nvp("TriggerType", trigger_);
  ar & make_nvp("TriggerSituation", mode_);

  ar & make_nvp("WhichATWD", whichATWD_);
  
  // since version 3 of the file the raw waveforms are stored in delta
  // compressed form. This is only done, if archived to a non XML archive,
  // to allow to inspect the waveforms with dataio_shovel.
  if(  ( typeid(ar) != typeid(icecube::archive::xml_oarchive) ) ){
    try{
      I3DeltaCompression::DeltaCompressor compressor;
      std::vector< std::vector<unsigned int> > compressedATWD;
      
      std::vector<std::vector<int> >::const_iterator it;
      for( it = rawATWD_.begin(); it < rawATWD_.end(); it++ ){
        // reset the compressor and compresse the waveform
        compressor.reset();
        compressor.compress( *it );
        
        // copy the compressed ATWD waveform to the compressed std::vector and 
        // add the std::vector to the fector of compressed ATWD waveforms.
        const std::vector<unsigned int>& compVals = compressor.getCompressed();
        std::vector<unsigned int> compVector( compVals.begin(), compVals.end() );
        compressedATWD.push_back( compVector );
      }
      // serialize all compressed ATWD waveforms
      ar & make_nvp("CompressedATWD", compressedATWD);
      
      // also write the number of samples for each ATWD waveform to the archive
      // as the decompressed waveforms need to be truncated from extra values
      // which are appended as the algorithm doesn't allow for a end marker.
      for( it = rawATWD_.begin(); it < rawATWD_.end(); it++ ){
        int numSamples = static_cast<int>( (*it).size() );
        ar & make_nvp( "NumSamplesATWD", numSamples );
      }
      
      // reset the compressor and compresse the waveform
      compressor.reset();
      compressor.compress( rawFADC_ );
      
      // copy the compressed waveform to the output std::vector and serialize this 
      // std::vector with the compressed FADC waveform.
      const std::vector<unsigned int>& compVals = compressor.getCompressed();
      std::vector<unsigned int> compressedFADC( compVals.begin(), compVals.end() );
      ar & make_nvp("CompressedFADC", compressedFADC );
      
      // also write the number of samples for this waveform to the archive
      // as the decompressed waveform needs to be truncated from extra values
      // which are appended as the algorithm doesn't allow for a end marker.
      int numSamples =  rawFADC_.size();
      ar & make_nvp("NumSamplesFADC", numSamples );
    }
    catch( const std::domain_error& ex ) {
      log_fatal("%s", ex.what());
    }
  }else{
    // since the serialization method is split we only need to 
    // worry about the load method so we can read old I3Vector data
    ar & make_nvp("RawATWD", rawATWD_);
    ar & make_nvp("RawFADC", rawFADC_);
  }
  ar & make_nvp("LocalCoincidence", localCoincidence_);
  ar & make_nvp("RawChargeStamp", rawChargeStamp_);
  ar & make_nvp("Pedestal", pedestal_);
  ar & make_nvp("ChargeStampHighestSample", chargeStampHighestSample_);    
  ar & make_nvp("RawATWDChargeStamp", rawATWDChargeStamp_);    
  ar & make_nvp("WhichATWDChargeStamp", whichATWDChargeStamp_);    
}

template <class Archive>
void I3DOMLaunch::load(Archive& ar, unsigned version)
{
  ar & make_nvp("StartTime", startTime_);
  // ignore the "Trigger" information of old data
  if(version < 1)
  {
    int tmp;
    ar & make_nvp("Trigger", tmp);
  }
  // for version > 0 there is a trigger type and mode to be serialized
  if(version > 0)
  {
    ar & make_nvp("TriggerType", trigger_);
    ar & make_nvp("TriggerSituation", mode_);
  }
  // for version <= 0 there is no trigger type and mode available
  // use default values instead
  else
  {
    trigger_ = TEST_PATTERN;
    mode_ = UNDEFINED;
  }
  ar & make_nvp("WhichATWD", whichATWD_);

  // since version 3 of the file the raw waveforms are stored in delta
  // compressed form. This is only done, if archived to a non XML archive,
  // to allow to inspect the waveforms with dataio_shovel.
  if( ( version > 2 )
      && 
      ( typeid(ar) != typeid(icecube::archive::xml_iarchive) ) )
  {
    try
    {
      I3DeltaCompression::DeltaCompressor compressor;
      
      // Create the data structure for the compressed version 
      // of the waveforms and deserialize fro the archive
      std::vector< std::vector<unsigned int> > compressedATWD( rawATWD_.size() );
      ar & make_nvp("CompressedATWD", compressedATWD);
      
      std::vector< std::vector<unsigned int> >::const_iterator it;
      std::vector<std::vector<int> >::iterator it2 = rawATWD_.begin();
      for( it = compressedATWD.begin(); it < compressedATWD.end(); it++, it2++ )
      {
        // reset compressor and initialize with the compressed data.
        compressor.reset();
        compressor.setCompressed( *it );
        
        // clear the rawATWD_ waveform and fill it with the decompressed values.
        (*it2).clear();
        compressor.decompress( dynamic_cast< std::vector<int>& >(*it2) );
        
        // get the number of real samples of the waveform and truncate any
        // extra values from the std::vector.
        unsigned int numSamples;
        ar & make_nvp( "NumSamplesATWD", numSamples );
	if (numSamples > it2->size())
	  {
	    log_fatal("error delta decompressing");
	  }


        (*it2).resize( numSamples );
      }
      
      std::vector<unsigned int> compressedFADC;
      ar & make_nvp("CompressedFADC", compressedFADC);

      // reset compressor and initialize with the compressed data.
      compressor.reset();
      compressor.setCompressed( compressedFADC );
      
      // clear the rawFADC_ waveform and fill it with the decompressed values.
      rawFADC_.clear();
      compressor.decompress( rawFADC_ );
      
      // get the number of real samples of the waveform and truncate any
      // extra values from the std::vector.
      unsigned int numFADCSamples;
      ar & make_nvp("NumSamplesFADC", numFADCSamples);
	if (numFADCSamples > rawFADC_.size())
	  {
	    log_fatal("error delta decompressing");
	  }

      rawFADC_.resize(numFADCSamples);
    }
    catch( const std::domain_error& ex )
    {
      log_fatal("%s", ex.what());
    }
  }
  else
  {
    if(version > 4){
      ar & make_nvp("RawATWD", rawATWD_);
      ar & make_nvp("RawFADC", rawFADC_);
    }else{
      // need to deserialize I3Vectors and translate to std::vector
      I3Vector< I3Vector<int> > tempRawATWD;
      ar & make_nvp("RawATWD", tempRawATWD);
      //need to clear the vectors before filling them
      rawATWD_.clear();
      BOOST_FOREACH(I3Vector<int>& wf, tempRawATWD){
	std::vector<int> tempWF;
	BOOST_FOREACH(int i, wf) tempWF.push_back(i);
	rawATWD_.push_back(tempWF);
      }

      //need to clear the vectors before filling them
      I3Vector<int> tempRawFADC;      
      ar & make_nvp("RawFADC", tempRawFADC);
      rawFADC_.clear();
      BOOST_FOREACH(int i, tempRawFADC) 
	rawFADC_.push_back(i);
    }
  }
  ar & make_nvp("LocalCoincidence", localCoincidence_);
  if( version > 4){
    ar & make_nvp("RawChargeStamp", rawChargeStamp_);
  }else{
    I3Vector<int> tempRawChargeStamp;
    ar & make_nvp("RawChargeStamp", tempRawChargeStamp);
    rawChargeStamp_.clear();
    BOOST_FOREACH(int i, tempRawChargeStamp) 
      rawChargeStamp_.push_back(i);    
  }
  if(version > 2)
    {
      ar & make_nvp("Pedestal", pedestal_);
    }
  // ignore the range for the raw charge stamp of old data
  if(version < 2)
  {
    bool tmp;
    ar & make_nvp("ChargeStampRange", tmp);
  }
  // for version > 1 there is an index to be serialized,
  // addressing the highest sample of the charge stamp
  if(version > 1)
  {
    ar & make_nvp("ChargeStampHighestSample", chargeStampHighestSample_);    
  }
  // for version <= 1 there is no index available
  // use a value of 0 instead - should be fine since rawChargeStamp_ is empty
  else
  {
    if(!rawChargeStamp_.empty()) log_warn("oops, coarse charge stamp is not empty");
    chargeStampHighestSample_ = 0u;
  }
  if(version>3)
    {
      ar & make_nvp("RawATWDChargeStamp", rawATWDChargeStamp_);    
      ar & make_nvp("WhichATWDChargeStamp", whichATWDChargeStamp_);    
    }
}

bool operator==(const I3DOMLaunch& lhs, const I3DOMLaunch& rhs){
  return ( lhs.GetStartTime() == rhs.GetStartTime() &&
	   lhs.GetTriggerType() == rhs.GetTriggerType() &&	  
	   lhs.GetTriggerMode() == rhs.GetTriggerMode() &&	  
	   lhs.GetWhichATWD() == rhs.GetWhichATWD() &&	  
	   lhs.GetRawATWD(0) == rhs.GetRawATWD(0) &&	  
	   lhs.GetRawATWD(1) == rhs.GetRawATWD(1) &&	  
	   lhs.GetRawATWD(2) == rhs.GetRawATWD(2) &&	  
	   lhs.GetRawATWD(3) == rhs.GetRawATWD(3) &&	  
	   lhs.GetRawFADC() == rhs.GetRawFADC() &&	  
	   lhs.GetLCBit() == rhs.GetLCBit() &&	  
	   lhs.GetIsPedestalSub() == rhs.GetIsPedestalSub() &&	  
	   lhs.GetRawChargeStamp() == rhs.GetRawChargeStamp() &&	  
	   lhs.GetChargeStampHighestSample() == rhs.GetChargeStampHighestSample() );
}

std::ostream& I3DOMLaunch::Print(std::ostream& oss) const
{
    oss << "[ I3DOMLaunch :\n"
    << "    StartTime : " << GetStartTime() << '\n'
    << "        LCBit : " << GetLCBit() << '\n'
    << "]" ;
    
    return oss;
}

std::ostream& operator<<(std::ostream& oss, const I3DOMLaunch& d)
{
    return(d.Print(oss));
}

I3_SPLIT_SERIALIZABLE(I3DOMLaunch);

I3_SERIALIZABLE(I3DOMLaunchSeriesMap);
