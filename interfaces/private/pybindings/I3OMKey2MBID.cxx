

#include "interfaces/I3OMKey2MBID.h"

namespace bp = boost::python;

struct I3OMKey2MBIDWrapper : public I3OMKey2MBID, bp::wrapper<I3OMKey2MBID> {
  bool OMKeyExists(long long int mbid) const
  {
    return this->get_override("OMKeyExists")(mbid);
  }
  bool MBIDExists(OMKey key) const
  {
    return this->get_override("MBIDExists")(key);
  }
  OMKey GetOMKey(long long int mbid) const
  {
    return this->get_override("GetOMKey")(mbid);
  }
  long long int GetMBID(OMKey key) const
  {
    return this->get_override("GetMBID")(key);
  }
};

void register_I3OMKey2MBID()
{
	bp::class_<I3OMKey2MBIDWrapper, boost::noncopyable>("I3OMKey2MBID")
	;
}
