#ifndef ICETRAY_PYTHON_OSTREAM_OVERLOADS_HPP_INCLUDED
#define ICETRAY_PYTHON_OSTREAM_OVERLOADS_HPP_INCLUDED

#include <vector>
#include <icetray/has_operator.h>

namespace I3{
namespace detail{
  template<typename T>
  std::ostream& print_vector(std::ostream& os, const std::vector<T> l, std::true_type){
    os << '[';
    bool first=true;
    for(const auto& item : l){
      if(first)
        first=false;
      else
        os << ", ";
      os << item;
    }
    os << ']';
    return os;
  }
  
  template<typename T>
  std::ostream& print_vector(std::ostream& os, const std::vector<T> l, std::false_type){
    os << '[' << l.size() << " element" << (l.size()==1?"":"s") << ']';
    return os;
  }
}
}

//Somewhat evil hack: Inject this overload into the standard namespace, 
//enabling it to be found by ADL when needed by code which may be in arbitrary
//other namespaces (e.g. boost::detail::variant). 
namespace std{
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T> l){
  constexpr bool can_print=has_operator::insertion<std::ostream&,T>::value;
  return(I3::detail::print_vector(os,l,std::integral_constant<bool,can_print>()));
}
}

template <typename T>
struct I3Vector;

// specialize I3Vector<char> because ...well it's special
std::ostream& operator<<(std::ostream& oss, const I3Vector<char> l);

#endif // ICETRAY_PYTHON_OSTREAM_OVERLOADS_HPP_INCLUDED
