#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/class.hpp>
#include <dataclasses/physics/I3Particle.h>
#include <icetray/serialization.h>

using namespace boost::python;

struct UnregisteredTrack : public I3Particle {
public:

  template <typename Archive>
  void serialize(Archive & ar, unsigned version){
    ar & make_nvp("I3Particle", base_object<I3Particle>(*this));
  }
};

I3_SERIALIZABLE(UnregisteredTrack);


BOOST_PYTHON_MODULE(test_unregistered){

  class_<UnregisteredTrack, 
         boost::shared_ptr<UnregisteredTrack>, 
         bases<I3Particle> >
    ("UnregisteredTrack")
    ;
}

