/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 */

#include <dataclasses/external/CompareFloatingPoint.h>
#include <dataclasses/physics/I3NoiseSuppressionRecord.h>


template <class Archive>
void I3NoiseSuppressionRecord::serialize(Archive& ar, unsigned int version) {
  if (version > i3noisesuppressionrecord_version_) {
    log_fatal("Attempting to read version %u from file"
              " but running version %u of I3NoiseSuppressionRecord class.",
              version, i3noisesuppressionrecord_version_);
  }
  ar & make_nvp("time", exclWindowStart_);
#ifdef I3NOISESUPPRESSIONRECORD_INCLUDE_SIZES
  ar & make_nvp("exclWindowSize", exclWindowSize_);
  ar & make_nvp("lcWindowSize", lcWindowSize_);
#endif  // I3NOISESUPPRESSIONRECORD_INCLUDE_SIZES
  ar & make_nvp("nNoiseHits", n_);
}

std::ostream& I3NoiseSuppressionRecord::Print(std::ostream& os) const {
  os << "I3NoiseSuppressionRecord(time=" << exclWindowStart_
     << ", n=" << n_ << ")";
  return os;
}

std::ostream& operator<<(std::ostream& os,
                         const I3NoiseSuppressionRecord& noise) {
  return noise.Print(os);
}

bool
I3NoiseSuppressionRecord::operator==(const I3NoiseSuppressionRecord& rhs) const {
  return (n_ == rhs.n_
          && CompareFloatingPoint::Compare(exclWindowStart_, rhs.exclWindowStart_)
#ifdef I3NOISESUPPRESSIONRECORD_INCLUDE_SIZES
          && CompareFloatingPoint::Compare(exclWindowSize_, rhs.exclWindowSize_)
          && CompareFloatingPoint::Compare(lcWindowSize_, rhs.lcWindowSize_)
#endif  // I3NOISESUPPRESSIONRECORD_INCLUDE_SIZES
         );
}


I3_SERIALIZABLE(I3NoiseSuppressionRecord);
I3_SERIALIZABLE(I3NoiseSuppressionRecordSeriesMap);
