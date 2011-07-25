#ifndef ICETRAY_PYTHON_ICETRAY_STREAM_OVERLOADS_HPP_INCLUDED
#define ICETRAY_PYTHON_ICETRAY_STREAM_OVERLOADS_HPP_INCLUDED

template <typename T, typename U>
std::ostream& operator<<(std::ostream& oss, std::pair<T, U> p){
  oss << "("<<p.first<<", "<<p.second<<")";
  return oss;
}

#endif // ICETRAY_PYTHON_ICETRAY_STREAM_OVERLOADS_HPP_INCLUDED
