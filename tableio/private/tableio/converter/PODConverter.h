/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> Last changed by: $LastChangedBy$
 */

#ifndef TABLEIO_PODCONVERTER_H_INCLUDED
#define TABLEIO_PODCONVERTER_H_INCLUDED

#include "tableio/I3Converter.h"
#include "dataclasses/I3Double.h"
#include "icetray/I3Bool.h"
#include "icetray/I3Int.h"

template<class FrmObj, typename TableType, const char* unit>
class PODConverter : public I3ConverterImplementation<FrmObj> {
    private:
        I3TableRowDescriptionPtr CreateDescription(const FrmObj& object) {
            I3TableRowDescriptionPtr desc = 
                I3TableRowDescriptionPtr(new I3TableRowDescription() );
            desc->AddField<TableType>("value", std::string(unit), "derived from a POD frame object");
            return desc;
        }

        size_t FillRows(const FrmObj& object, I3TableRowPtr rows) {
            rows->Set<TableType>("value", object.value);
            return 1;
        }
};

char PODConverter_NoUnit[] = "";
char PODConverter_BoolUnit[] = "bool";

typedef PODConverter<I3Double,double , PODConverter_NoUnit> I3DoubleConverter;
typedef PODConverter<I3Int   ,int32_t, PODConverter_NoUnit> I3IntConverter;
typedef PODConverter<I3Bool  ,bool   , PODConverter_BoolUnit> I3BoolConverter;

#endif // TABLEIO_PODCONVERTER_H_INCLUDED
