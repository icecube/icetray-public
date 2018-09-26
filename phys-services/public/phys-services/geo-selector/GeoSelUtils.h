/**
 * copyright  (C) 2004
 * the IceCube collaboration
 *  $Id$
  *
 * @file GeoSelUtils.h
 * @version $Revision$
 * @date $Date$
 * @author olivas
 */

#ifndef I3GEOSELUTILITIES_H
#define I3GEOSELUTILITIES_H

/**
 * @brief Utilities for parsing the input parameters for I3GeometrySelector.
 */

#include <vector>
#include <string>
#include "dataclasses/geometry/I3Geometry.h"

#include <dataclasses/I3Map.h>

namespace geo_sel_utils{

  /**
   *Takes in two std::strings. (strings to use and strings to include)
   *The std::strings can take the form "i,j,n:m" (and any permutation) 
   *which would ex/include std::strings i,j, and n through m.
   */
  std::vector<int> make_good_strings(const std::string, const std::string);

  /**
   *checks whether an element (type T) exists in a std::vector
   *Returns true if it does, false otherwise
   */
  template <class T> bool exists(const T& , const std::vector<T>& );

  /**
   *checks whether an element (type T) exists in a std::vector
   *Returns a const_iterator (i.e. postion), which is equal to
   *'end' if the element doesn't exist
   */
  template <class T> typename std::vector<T>::const_iterator 
    exists_at(const T& , const std::vector<T>& );


  /**
   *Takes in a std::string of the form "i,j,n:m" and returns a vector
   *of integers whose elements are "i,j,n,n+1,n+2,...,m-1,m"
   */
  std::vector<int> parse_string_list(const std::string&);

  /**
   *Checks to see whether a std::string contains characters other than
   *signed integers, and the two delimiters used ',' and ':' 
   */
  bool good_input(const std::string& );

  template<class T>
  boost::shared_ptr<I3Map<OMKey,T> >
    map_cleaner(boost::shared_ptr<const I3Map<OMKey,T> >, std::vector<int>);

  /**
   * Calculates the center of the detector in X-Y.
   * Returns a std::pair<double,double> where first
   * is x and second is y.
   */
  std::pair<double,double> detector_center(I3GeometryConstPtr , 
					   const std::vector<int>& );
}

#endif
