#include <icetray/serialization.h>
#include <dataclasses/physics/I3FlasherInfo.h>

template <class Archive>
void I3FlasherInfo::serialize(Archive& ar, unsigned version){
    if (version>i3flasherinfo_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3FlasherInfo class.",version,i3flasherinfo_version_);
    
    ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
    ar & make_nvp("FlasherOM", flasherOM_);
    ar & make_nvp("FlashTime", flashTime_);
    ar & make_nvp("ATWDBinSize", aTWDBinSize_);
    ar & make_nvp("RawATWD3", rawATWD3_);
    ar & make_nvp("mask", mask_);
    ar & make_nvp("LEDBrightness", LEDBrightness_);
    ar & make_nvp("width", width_);
    if(version>0)
	    ar & make_nvp("rate", rate_);
}

std::ostream& I3FlasherInfo::Print(std::ostream& os) const{
    os << "[FlasherInfo:"
       << "\n  Flashing OM : " << flasherOM_
       << "\n   Flash time : " << flashTime_
       << "\n     LED mask : ";
    for(std::size_t i=0; i<12; i++)
		os << ((mask_ & (1u<<i)) ? '1' : '0');
    os << "\n   Brightness : " << LEDBrightness_
       << "\n        Width : " << width_
       << "\n         Rate : " << rate_
       << "\n]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const I3FlasherInfo& fi){
    return(fi.Print(os));
}

I3_SERIALIZABLE(I3FlasherInfo);
I3_SERIALIZABLE(I3FlasherInfoVect);
