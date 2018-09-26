#ifndef I3CUTVALUESCONVERTER_H_INCLUDED
#define I3CUTVALUESCONVERTER_H_INCLUDED

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

#include <tableio/I3Converter.h>
#include <phys-services/I3CutValues.h>

class I3CutValuesConverter : public I3ConverterImplementation<I3CutValues > {

private:
    I3TableRowDescriptionPtr CreateDescription(const I3CutValues& cv);
    size_t FillRows(const I3CutValues& cv, I3TableRowPtr rows);
    
};

#endif  // I3CUTVALUESCONVERTER_H_INCLUDED
