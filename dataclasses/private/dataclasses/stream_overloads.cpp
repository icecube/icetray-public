#include <dataclasses/I3Vector.h>

// specialize I3Vector<char> because ...well it's special
std::ostream& operator<<(std::ostream& oss, const I3Vector<char> l){
  oss << std::string(l.begin(),l.end()); 
 return oss;
}

