#include <dataclasses/IceActKey.h>
#include <icetray/I3Logging.h>

std::ostream& operator<<(std::ostream& os, const IceActKey& key) {
  os << "IceActKey(" << key.GetStationID() << "," << key.GetTelescopeID() << ")";
  return os;
}

template <class Archive>
void
IceActKey::serialize(Archive& ar, unsigned version) {
  if (version > iceactkey_version_)
    log_fatal("Attempting to read version %u from file but running version %u of IceActKey class.",
              version, iceactkey_version_);

  ar & make_nvp("StationID", stationID_);
  ar & make_nvp("TelescopeID", telescopeID_);
}

bool operator<(const IceActKey& lhs, const IceActKey& rhs) {
  if (lhs.GetStationID() < rhs.GetStationID()) {
    return true;
  } else if (lhs.GetStationID() > rhs.GetStationID()) {
    return false;
  } else if (lhs.GetTelescopeID() < rhs.GetTelescopeID()) {
    return true;
  } else if (lhs.GetTelescopeID() > rhs.GetTelescopeID()) {
    return false;
  } else {
    return false;
  }
}

I3_SERIALIZABLE(IceActKey);
I3_SERIALIZABLE(I3VectorIceActKey);
