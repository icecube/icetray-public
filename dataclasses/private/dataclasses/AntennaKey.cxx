#include <dataclasses/AntennaKey.h>
#include <icetray/I3Logging.h>

std::ostream& operator<<(std::ostream& os, const AntennaKey& key) {
  os << "AntennaKey(" << key.GetStationID() << "," << key.GetAntennaID() << ")";
  return os;
}

template <class Archive>
void
AntennaKey::serialize(Archive& ar, unsigned version) {
  if (version > antennakey_version_)
    log_fatal("Attempting to read version %u from file but running version %u of AntennaKey class.",
              version, antennakey_version_);

  ar & make_nvp("StationID", stationID_);
  ar & make_nvp("AntennaID", antennaID_);
}

bool operator<(const AntennaKey& lhs, const AntennaKey& rhs) {
  if (lhs.GetStationID() < rhs.GetStationID()) {
    return true;
  } else if (lhs.GetStationID() > rhs.GetStationID()) {
    return false;
  } else if (lhs.GetAntennaID() < rhs.GetAntennaID()) {
    return true;
  } else if (lhs.GetAntennaID() > rhs.GetAntennaID()) {
    return false;
  } else {
    return false;
  }
}

I3_SERIALIZABLE(AntennaKey);
I3_SERIALIZABLE(I3VectorAntennaKey);
