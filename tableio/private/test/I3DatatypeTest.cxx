/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$
 */

#include <I3Test.h>
#include "tableio/I3Datatype.h"
#include "tableio/I3TableRowDescription.h"

TEST_GROUP(I3DatatypeTests)

TEST(signedness) {
  I3Datatype stype = I3DatatypeFromNativeType<int>();
  I3Datatype utype = I3DatatypeFromNativeType<unsigned int>();
  I3Datatype doubletype = I3DatatypeFromNativeType<double>();
    
    ENSURE_EQUAL( stype.is_signed, true, "Signed types are detected");
    ENSURE_EQUAL( utype.is_signed, false, "Unsigned types are detected");
    
    ENSURE_EQUAL( doubletype.size, sizeof(double), "Sizes are correct" );
    
    ENSURE_EQUAL( (int)doubletype.kind, (int)I3Datatype::Float, "Floats are recognized");
    ENSURE( (int)doubletype.kind != (int)I3Datatype::Int, "Floats are recognized");
    ENSURE( (int)doubletype.kind != (int)I3Datatype::Bool, "Floats are recognized");
    ENSURE( (int)doubletype.kind != (int)I3Datatype::Enum, "Floats are recognized");
    
    ENSURE_EQUAL( stype.size, sizeof(int), "Sizes are correct" );
    
    ENSURE_EQUAL( (int)stype.kind, (int)I3Datatype::Int, "Ints are recognized");
    ENSURE( (int)stype.kind != (int)I3Datatype::Float,   "Ints are recognized");
    ENSURE( (int)stype.kind != (int)I3Datatype::Bool,    "Ints are recognized");
    ENSURE( (int)stype.kind != (int)I3Datatype::Enum,    "Ints are recognized");
    
    I3Datatype btype = I3DatatypeFromNativeType<bool>();
    ENSURE_EQUAL( (int)btype.kind, (int)I3Datatype::Bool, "Bools are recognized");
    ENSURE_EQUAL( btype.size, sizeof(bool), "Sizes are correct" );

}

TEST(labeling) {
    I3Datatype ltype = I3DatatypeFromNativeType<long>();
    ENSURE_EQUAL( ltype.description, std::string("long"), "Labels!");
    I3Datatype ultype = I3DatatypeFromNativeType<unsigned long>();
    ENSURE_EQUAL( ultype.description, std::string("unsigned long"), "Labels!");
}

struct OutThere {
    enum Yonkers { Hick, Town, Barnaby };
};

TEST(enums) {
    MAKE_ENUM_VECTOR(members,OutThere,Yonkers,(Hick)(Town)(Barnaby));
    I3Datatype etype = I3DatatypeFromNativeType<OutThere::Yonkers>(members);
    ENSURE_EQUAL( (int)etype.kind, (int)I3Datatype::Enum );
    ENSURE_EQUAL( etype.description, std::string("OutThere::Yonkers"), "Labels!");
    ENSURE_EQUAL( etype.enum_members.at(0).second, (long)OutThere::Hick);
    ENSURE_EQUAL( etype.enum_members.at(1).second, (long)OutThere::Town);
    ENSURE_EQUAL( etype.enum_members.at(2).second, (long)OutThere::Barnaby);
    ENSURE_EQUAL( etype.enum_members.at(0).first, std::string("Hick") );
    ENSURE_EQUAL( etype.enum_members.at(1).first, std::string("Town") );
    ENSURE_EQUAL( etype.enum_members.at(2).first, std::string("Barnaby") );
}
