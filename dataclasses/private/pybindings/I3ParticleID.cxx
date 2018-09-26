#include <dataclasses/physics/I3ParticleID.h>
#include <icetray/python/dataclass_suite.hpp>

using namespace boost::python;

namespace {

int hash_id(const I3ParticleID &id){
	return hash<I3ParticleID>()(id);
}

}

void register_I3ParticleID()
{
	class_<I3ParticleID, boost::shared_ptr<I3ParticleID> >("I3ParticleID")
	.def(init<I3ParticleID>())
	.def(init<uint64_t,int32_t>())
	.def_readwrite("majorID",&I3ParticleID::majorID)
	.def_readwrite("minorID",&I3ParticleID::minorID)
	.def(dataclass_suite<I3ParticleID>())
	.def("__hash__", &hash_id)
	;
}
