/**
 * copyright  (C) 2012
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Martin Wolf <martin.wolf@fysik.su.se> $LastChangedBy$
 */

#ifndef TABLEIO_I3POSITIONCONVERTER_H_INCLUDED
#define TABLEIO_I3POSITIONCONVERTER_H_INCLUDED

#include <string>

#include "tableio/I3Converter.h"
#include "dataclasses/I3Position.h"

class I3PositionConverter : public I3ConverterImplementation<I3Position>
{
    public:
        enum BookRefFrame { car = 0, sph = 1, cyl = 2, all = 3 };

        //______________________________________________________________________
        I3PositionConverter()
          : bookRefFrame_(I3PositionConverter::car),
            tableColumnNamePrefix_("")
        {}

        //______________________________________________________________________
        /** The bookRefFrame argument specifies the reference frame for which
         *  position data should be booked.
         *  The tableColumnNamePrefix argument specified the prefix string that
         *  should be prefixed to all table column names.
         */
        I3PositionConverter(
            BookRefFrame bookRefFrame,
            std::string tableColumnNamePrefix
        )
          : bookRefFrame_(bookRefFrame),
            tableColumnNamePrefix_(tableColumnNamePrefix)
        {}

        //______________________________________________________________________
        I3TableRowDescriptionPtr
        CreateDescription(const I3Position& position);

        //______________________________________________________________________
        size_t
        FillRows(const I3Position& position, I3TableRowPtr row);

    protected:
        /** The enum value in what reference frame the position should get
         *  booked.
         */
        BookRefFrame bookRefFrame_;

        /** The prefix string that should be prefixed to all table column names.
         */
        std::string tableColumnNamePrefix_;
};

#endif // TABLEIO_I3POSITIONCONVERTER_H_INCLUDED
