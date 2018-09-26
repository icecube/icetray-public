#ifndef DEPRECATED_CLASSES_H_INCLUDED
#define DEPRECATED_CLASSES_H_INCLUDED

struct TWRAOMStatus {
  enum CableType{ELECTRICAL = 0, OPTICAL = 10};
  double binSize;
  unsigned int stopDelay;
  unsigned int baseline;
  unsigned int threshold;
  CableType cableType;
  
  friend class icecube::serialization::access;
  template <class Archive> void serialize (Archive& ar, unsigned version){
    ar & make_nvp("BinSize", binSize);
    ar & make_nvp("StopDelay", stopDelay);
    ar & make_nvp("Baseline", baseline);
    ar & make_nvp("Threshold", threshold);
    ar & make_nvp("CableType", cableType);
  }

};
I3_SERIALIZABLE(TWRAOMStatus);
#endif // DEPRECATED_CLASSES_H_INCLUDED
