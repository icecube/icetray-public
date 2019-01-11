#include <icetray/serialization.h>
#include <icetray/I3Bool.h>

I3Bool::I3Bool() : value(false) { }

I3Bool::I3Bool(bool b) : value(b) { }

I3Bool::I3Bool(const I3Bool& rhs) : value(rhs.value) { }

I3Bool& 
I3Bool::operator=(const I3Bool& rhs)
{
  // careful of self-assignment
  if (this != &rhs)
    value = rhs.value;

  return *this;
}

template <class Archive>
void
I3Bool::serialize(Archive& ar,unsigned)
{
  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("value", value);
}

I3_SERIALIZABLE(I3Bool);

