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

#ifndef TABLEIO_I3FILTERRESULTMAPCONVERTER_H_INCLUDED
#define TABLEIO_I3FILTERRESULTMAPCONVERTER_H_INCLUDED

#include "tableio/I3Converter.h"
#include "dataclasses/physics/I3FilterResult.h"

class I3FilterResultMapConverter : public I3ConverterImplementation<I3FilterResultMap> {
    private:
        I3TableRowDescriptionPtr CreateDescription(const I3FilterResultMap& frmap);
        size_t FillRows(const I3FilterResultMap& frmap, I3TableRowPtr rows);
};

#endif // TABLEIO_I3FILTERRESULTMAPCONVERTER_H_INCLUDED
