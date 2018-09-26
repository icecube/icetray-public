
#include <icetray/serialization.h>
#include <dataclasses/status/I3FlasherStatus.h>

template <class Archive>
void I3FlasherStatus::serialize(Archive& ar, unsigned version) {

  if (version > i3flasherstatus_version_) {
    log_fatal("Attempting to read version %u from file "
              "but running version %u of I3FlasherStatus "
              "class.", version, i3flasherstatus_version_);
  }

  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("brightness", brightness);
  ar & make_nvp("window", window);
  ar & make_nvp("rate", rate);
  ar & make_nvp("mask", mask);
  ar & make_nvp("delay", delay);
}

I3_SERIALIZABLE(I3FlasherStatus);
I3_SERIALIZABLE(I3FlasherStatusMap);
I3_SERIALIZABLE(I3FlasherSubrunMap);

std::ostream& operator<<(std::ostream& oss, const I3FlasherStatus& s) {
  oss << "[ I3FlasherStatus  :: " << std::endl
	  << "Brightness : " << s.brightness << std::endl
	  << "    Window : " << s.window << std::endl
	  << "      Rate : " << s.rate << std::endl
	  << "      Mask : " << s.mask << std::endl
	  << "     Delay : " << s.delay << std::endl
	  << "]";
  return oss;
}

std::ostream& operator<<(std::ostream& oss, const I3FlasherStatusMap& map) {
  oss << "[ I3FlasherStatusMap :: " << std::endl;
  for (I3FlasherStatusMap::const_iterator iter = map.begin();
    		                              iter != map.end(); ++iter) {
    oss << "  " << iter->first << " : " << iter->second << std::endl;
  }
  oss << "]";
  return oss;
}
std::ostream& operator<<(std::ostream& oss, const I3FlasherSubrunMap& map) {
  oss << "[ I3FlasherSubrunMap :: " << std::endl;
  for (I3FlasherSubrunMap::const_iterator iter = map.begin();
		                                  iter != map.end(); ++iter) {
    oss << "  " << iter->first << " : " << iter->second << std::endl;
  }
  oss << "]";
  return oss;
}
