/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Fabian Kislat <fabian.kislat@desy.de> $LastChangedBy$
 */

#ifndef TABLEIO_POD_CONVERTER_TYPE_MAPPING_H_INCLUDED
#define TABLEIO_POD_CONVERTER_TYPE_MAPPING_H_INCLUDED

/// @cond
namespace detail {

  template <typename T>
  struct pod_converter_type_mapping {
    typedef T type;
  };
    
  template <>
  struct pod_converter_type_mapping<signed short> {
    typedef int16_t type;
  };
    
  template <>
  struct pod_converter_type_mapping<signed int> {
    typedef int32_t type;
  };
    
  template <>
  struct pod_converter_type_mapping<signed long> {
    typedef int64_t type;
  };
    
  template <>
  struct pod_converter_type_mapping<signed long long> {
    typedef int64_t type;
  };
    
  template <>
  struct pod_converter_type_mapping<unsigned short> {
    typedef uint16_t type;
  };

  template <>
  struct pod_converter_type_mapping<unsigned int> {
    typedef uint32_t type;
  };
    
  template <>
  struct pod_converter_type_mapping<unsigned long> {
    typedef uint64_t type;
  };
    
  template <>
  struct pod_converter_type_mapping<unsigned long long> {
    typedef uint64_t type;
  };
    
}
/// @endcond

#endif // TABLEIO_POD_CONVERTER_TYPE_MAPPING_H_INCLUDED
