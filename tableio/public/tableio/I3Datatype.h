/*
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$
 */

#ifndef I3DATATYPE_H_ZO84X7OI
#define I3DATATYPE_H_ZO84X7OI

#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>

#include <vector>
#include <map>
#include <string>
#include <cctype>

#include <I3/name_of.h>

/**
 * \brief Represents an atomic datatype
 */
struct I3Datatype {
  /**
   * \brief Categories describing types of atomic data objects
   */
  enum TypeClass {
    Float = 0,  /*!< A floating point number */
    Int   = 1,  /*!< An integer type number */
    Enum  = 2,  /*!< An enumeration */
    Bool  = 3   /*!< A boolean value */
  };
  TypeClass kind;  ///< The type class.
  size_t size;     ///< The size of the data type in memory.
  bool is_signed;  ///< True if the data type has a sign bit.
  /**
   * \brief Associates enum members with a string.
   *
   * This information is used by tableio to write strings into
   * tables instead of simply the integer numbers associated with
   * the enumeration.
   */
  std::vector<std::pair<std::string,long> > enum_members;
  std::string description;  ///< A description of the data type
  bool operator!=(const I3Datatype& rhs) const;
  bool operator==(const I3Datatype& rhs) const;
  /**
   * \brief Tests if two data types are compatible
   */
  bool CompatibleWith(const I3Datatype& rhs, bool enums_are_ints = false) const;
  std::string TypeClassAsString() const;
  std::string AsString() const;

  I3Datatype() {};
  I3Datatype(TypeClass k, size_t s, bool sign) : kind(k),size(s),is_signed(sign) {};
};


/// Generates an I3Datatype data description object from a native type
template <typename T>
I3Datatype I3DatatypeFromNativeType() 
{
  std::string label = I3::name_of<T>();
  BOOST_STATIC_ASSERT(boost::is_pod<T>::value);

    I3Datatype dtype;
    dtype.size = sizeof(T);
    
    if (boost::is_integral<T>()) {
        dtype.kind = I3Datatype::Int;
        dtype.is_signed = boost::is_signed<T>();
    } else if (boost::is_enum<T>()) {
        dtype.kind = I3Datatype::Enum;
        dtype.is_signed = boost::is_signed<T>();
    } else { // it's a float
        dtype.kind = I3Datatype::Float;
        dtype.is_signed = true;
    }
    dtype.description = label;
    return dtype;
};

/// Generates an I3Datatype describing a native bool.
template <>
I3Datatype I3DatatypeFromNativeType<bool>();

/**
 * \brief Generates an I3Datatype object from an enum type
 *
 * This is an overload of the function above to treat enumerations.
 *
 * \param enum_labels Passing the labels associated with each enum value
 * to this function allows tableio to store these labels in the output
 * file.
 */
template <typename T>
static I3Datatype I3DatatypeFromNativeType(const std::vector<std::pair< std::string, T> >& enum_labels) 
{
    BOOST_STATIC_ASSERT(boost::is_enum<T>::value);

    I3Datatype dtype;
    dtype.size = sizeof(T);
    dtype.kind = I3Datatype::Enum;
    dtype.is_signed = true; // some dataclasses use signed values in enums
   
    dtype.description = I3::name_of<T>();
    // copy the enum members into the map
    typename std::vector<std::pair<std::string,T> >::const_iterator it;
    for (it = enum_labels.begin(); it != enum_labels.end(); it++) {
        dtype.enum_members.push_back(std::make_pair(it->first,long(it->second)));
    }
    return dtype;
};

#endif
