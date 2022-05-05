#include <dataclasses/ScintKey.h>
#include <icetray/I3Logging.h>

std::ostream& operator<<(std::ostream& os, const ScintKey& key) {
  os << "ScintKey(" << key.GetStationID() << "," << key.GetPanelID() << ")";
  return os;
}

template <class Archive>
void
ScintKey::serialize(Archive& ar, unsigned version) {
  if (version > scintkey_version_)
    log_fatal("Attempting to read version %u from file but running version %u of ScintKey class.",
              version, scintkey_version_);

  ar & make_nvp("StationID", stationID_);
  ar & make_nvp("PanelID", panelID_);
}

bool operator<(const ScintKey& lhs, const ScintKey& rhs) {
  if (lhs.GetStationID() < rhs.GetStationID()) {
    return true;
  } else if (lhs.GetStationID() > rhs.GetStationID()) {
    return false;
  } else if (lhs.GetPanelID() < rhs.GetPanelID()) {
    return true;
  } else if (lhs.GetPanelID() > rhs.GetPanelID()) {
    return false;
  } else {
    return false;
  }
}

I3_SERIALIZABLE(ScintKey);
I3_SERIALIZABLE(I3VectorScintKey);
