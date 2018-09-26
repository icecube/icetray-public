#include <icetray/serialization.h>
#include <dataclasses/I3String.h>

std::ostream& operator<<(std::ostream& oss, const I3String& s){
  oss << "I3String(\"" << s.value << "\")";
  return oss;
}
bool operator==(const I3String& lhs, std::string rhs){
  return lhs.value == rhs;
}
bool operator!=(const I3String& lhs, std::string rhs){
  return lhs.value != rhs;
}

I3_SERIALIZABLE(I3String);

