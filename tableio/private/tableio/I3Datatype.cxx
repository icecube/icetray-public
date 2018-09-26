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

#include "tableio/I3Datatype.h"

// specialization for boolean types
template <>
I3Datatype I3DatatypeFromNativeType<bool>() {
  std::string label("bool");
    I3Datatype dtype;
    dtype.size = sizeof(bool);
    dtype.kind = I3Datatype::Bool;
    dtype.is_signed = false;
    dtype.description = label;
    return dtype;
};

bool I3Datatype::operator!=(const I3Datatype& rhs) const {
      return !(*this == rhs);
};

bool I3Datatype::operator==(const I3Datatype& rhs) const {
      return ((kind == rhs.kind) &&
              (size == rhs.size) &&
              (is_signed == rhs.is_signed) &&
              (enum_members == rhs.enum_members));
};

// return true if the types can be safely reinterpret_cast'd() into
// each other. Note that this doesn't check the members of the enum
bool I3Datatype::CompatibleWith(const I3Datatype& rhs, bool enums_are_ints ) const {
    bool rep = (size == rhs.size) && (is_signed == rhs.is_signed);
    bool type = (kind == rhs.kind);
    if (enums_are_ints && 
        ((kind == I3Datatype::Enum || kind == I3Datatype::Int) &&
         (rhs.kind == I3Datatype::Enum || rhs.kind == I3Datatype::Int)))
         type = true;
    return (rep && type); 
};

std::string I3Datatype::TypeClassAsString() const {
    std::string out;
    switch(kind) {
        case Float: out = "Float"; break;
        case Int:   out = "Int"; break;
        case Enum:  out = "Enum"; break;
        case Bool:  out = "Bool"; break;
    }
    return out;
};

std::string I3Datatype::AsString() const {
    std::ostringstream out;
    out << "I3Datatype(";
    out << "kind=" << TypeClassAsString();
    out << " size=" << size;
    out << " signed=";
    
    if (is_signed) out << "True";
    else out << "False";
    
    out << ")";
    return out.str();
};
