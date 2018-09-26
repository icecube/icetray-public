#include <icetray/serialization.h>
#include <dataclasses/I3Vector.h>
#include <dataclasses/physics/I3Particle.h>

class I3MCList : public I3Vector<I3Particle> {
 public:
  ~I3MCList(){};

 private:
  friend class icecube::serialization::access;
  template <class Archive> void serialize(Archive & ar, unsigned version){
    log_error("I3MCList is deprecated.");
    ar & make_nvp("I3MCList", base_object< I3Vector<I3Particle> >(*this));
  }
};
I3_SERIALIZABLE(I3MCList);
