/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> $LastChangedBy$
 */

#ifndef TABLEIO_I3EVENTHEADERCONVERTER_HPP_INCLUDED
#define TABLEIO_I3EVENTHEADERCONVERTER_HPP_INCLUDED

#include "tableio/I3Converter.h"
#include "dataclasses/physics/I3EventHeader.h"

class I3EventHeaderConverter : public I3ConverterImplementation<I3EventHeader > {
private:
    I3TableRowDescriptionPtr CreateDescription(const I3EventHeader & params); 
    size_t FillRows(const I3EventHeader& params, I3TableRowPtr rows);
};
    
#endif // TABLEIO_I3EVENTHEADERCONVERTER_HPP_INCLUDED
