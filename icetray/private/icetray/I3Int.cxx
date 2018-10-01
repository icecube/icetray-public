#include <icetray/serialization.h>
#include <icetray/I3Int.h>

I3Int::I3Int() : value(false) { }

I3Int::I3Int(int i) : value(i) { }

I3Int::I3Int(const I3Int& rhs) : value(rhs.value) { }

I3Int& 
I3Int::operator=(const I3Int& rhs)
{
  // careful of self-assignment
  if (this != &rhs)
    value = rhs.value;

  return *this;
}

template <class Archive>
void
I3Int::serialize(Archive& ar,unsigned)
{
  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("value", value);
}

I3_SERIALIZABLE(I3Int);

