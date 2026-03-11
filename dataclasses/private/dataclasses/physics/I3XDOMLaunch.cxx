/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 */

#include <dataclasses/external/CompareFloatingPoint.h>
#include <dataclasses/physics/detail/delta-compression.h>
#include <dataclasses/physics/detail/I3XDOMLaunch.h>


//#define TEST_DELTA_COMPRESSION_DO_PRINT
#ifdef TEST_DELTA_COMPRESSION_DO_PRINT
#include <iomanip>
#include <iostream>
using std::cout;
using std::endl;
using std::hex;
using std::dec;
using std::setw;
using std::setfill;
#endif

using i3::dataclasses::detail::deltacompression::uncompress;
using i3::dataclasses::detail::deltacompression::compress;


#ifdef TEST_DELTA_COMPRESSION_DO_PRINT
namespace {


template <class T>
void print_vector(const std::vector<T>& data, const char* fn) {
  auto size = sizeof(T);
  auto width = 2 * size;
  auto mask = (0x01 << 8 * size) - 1;
  cout << endl << fn << ": ";
  for (unsigned int i = 0; i < data.size(); ++i) {
    cout << "0x" << setfill('0') << setw(width) << hex;
    if (i < data.size() - 1)
      cout << (static_cast<unsigned int>(data[i]) & mask) << dec << ", ";
    else
      cout << (static_cast<unsigned int>(data[i]) & mask) << dec << endl;
  }
}


}
#endif


void I3XDOMLaunch::Compress() const {
  try {
#ifdef TEST_DELTA_COMPRESSION_DO_PRINT
    print_vector(adcCache_, "Compress() [IN]");
#endif
    compress(adcData_, adcCache_, startBitsPerWord_, maxBitsPerWord_);
#ifdef TEST_DELTA_COMPRESSION_DO_PRINT
    print_vector(adcData_, "Compress() [OUT]");
#endif
    nSamples_ = adcCache_.size();
  } catch (...) {
    adcData_.clear();
    nSamples_ = 0;
    throw;
  }
}

void I3XDOMLaunch::Uncompress() const {
  try {
#ifdef TEST_DELTA_COMPRESSION_DO_PRINT
    print_vector(adcData_, "Uncompress() [IN]");
#endif
    adcCache_.resize(nSamples_, 0);
    uncompress(adcCache_, adcData_, startBitsPerWord_, maxBitsPerWord_);
#ifdef TEST_DELTA_COMPRESSION_DO_PRINT
    print_vector(adcCache_, "Uncompress() [OUT]");
#endif
  } catch (...) {
    adcCache_.clear();
    throw;
  }
}

const std::vector<int>& I3XDOMLaunch::GetADCData() const {
  if (adcCache_.empty() && !adcData_.empty() && nSamples_) {
      Uncompress();
  }
  return adcCache_;
}

template <class Archive>
void I3XDOMLaunch::save(Archive& ar, unsigned int version) const
{
  ar & make_nvp("time", time_);
  ar & make_nvp("lc", lc_);
  if (adcData_.empty() && !adcCache_.empty()) {
    Compress();
  }
  ar & make_nvp("adcData", adcData_);
  ar & make_nvp("nSamples", nSamples_);
  ar & make_nvp("nPreSamples", nPreSamples_);
  ar & make_nvp("baselineValue", baselineValue_);
  ar & make_nvp("fexResult", fexResult_);
  ar & make_nvp("selection", selectionType_);
}

template <class Archive>
void I3XDOMLaunch::load(Archive& ar, unsigned int version)
{
  if (version > i3xdomlaunch_version_) {
    log_fatal("Attempting to read version %u from file"
              " but running version %u of I3XDOMLaunch class.",
              version, i3xdomlaunch_version_);
  }
  ar & make_nvp("time", time_);
  if (version == 0){
    if(!warned_old_lc_){
      log_warn("Reading version 0 of I3XDOMLaunch but running"
               " version %u of the I3XDOMLaunch where new LC"
               " flags have been implemented. Ignoring existing LC.",
               i3xdomlaunch_version_);
      warned_old_lc_ = true;
    }
    bool old_lc = false;
    ar & make_nvp("lc", old_lc);
  }
  else{
    ar & make_nvp("lc", lc_);
  }
  ar & make_nvp("adcData", adcData_);
  ar & make_nvp("nSamples", nSamples_);
  ar & make_nvp("nPreSamples", nPreSamples_);
  ar & make_nvp("baselineValue", baselineValue_);
  ar & make_nvp("fexResult", fexResult_);
  ar & make_nvp("selection", selectionType_);
}

std::ostream& I3XDOMLaunch::Print(std::ostream& os) const {
  os << "I3XDOMLaunch(time=" << time_.first
     << ", lc=" << (uint) lc_;
  if (!adcData_.empty()) {
    os << ", #samples=" << nSamples_;
  } else {
    os << ", #samples=" << adcCache_.size();
  }
  os << ", #presamples=" << nPreSamples_
     << ", baseline=" << baselineValue_.first << ")";
  return os;
}

std::ostream& operator<<(std::ostream& os, const I3XDOMLaunch& launch) {
  return launch.Print(os);
}

bool
I3XDOMLaunch::operator==(const I3XDOMLaunch& rhs) const {
  bool retVal = (time_.second == rhs.time_.second &&
                 CompareFloatingPoint::Compare(time_.first, rhs.time_.first) &&
                 lc_ == rhs.lc_ &&
                 nPreSamples_ == rhs.nPreSamples_ &&
                 baselineValue_.second == rhs.baselineValue_.second &&
                 CompareFloatingPoint::Compare(baselineValue_.first,
                                               rhs.baselineValue_.first) &&
                 fexResult_ == rhs.fexResult_ &&
                 selectionType_ == rhs.selectionType_);
  if (retVal) {
    if (!adcCache_.empty() && !rhs.adcCache_.empty()) {
      retVal = adcCache_ == rhs.adcCache_;
    } else if(!adcData_.empty() && !rhs.adcData_.empty()) {
      retVal = (nSamples_ == rhs.nSamples_ &&
                adcData_ == rhs.adcData_);
    } else if (!adcData_.empty() && !rhs.adcCache_.empty()) {
      Uncompress();
      retVal = adcCache_ == rhs.adcCache_;
    } else if (!adcCache_.empty() && !rhs.adcData_.empty()) {
      Compress();
      retVal = (nSamples_ == rhs.nSamples_ &&
                adcData_ == rhs.adcData_);
    } else {
      retVal = (adcCache_.empty() && adcData_.empty() &&
                rhs.adcCache_.empty() && rhs.adcData_.empty());
    }
  }
  return retVal;
}


I3_SPLIT_SERIALIZABLE(I3XDOMLaunch);
