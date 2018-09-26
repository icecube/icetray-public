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

#include "tableio/converter/I3PositionConverter.h"

//______________________________________________________________________________
I3TableRowDescriptionPtr
I3PositionConverter::
CreateDescription(const I3Position& position)
{
    I3TableRowDescriptionPtr desc(new I3TableRowDescription());

    if(bookRefFrame_ == I3PositionConverter::car)
    {
        desc->AddField<double>(tableColumnNamePrefix_+"x", "m", "x-component of position in the cartesian reference frame");
        desc->AddField<double>(tableColumnNamePrefix_+"y", "m", "y-component of position in the cartesian reference frame");
        desc->AddField<double>(tableColumnNamePrefix_+"z", "m", "z-component of position in the cartesian reference frame");
    }
    else if(bookRefFrame_ == I3PositionConverter::cyl)
    {
        desc->AddField<double>(tableColumnNamePrefix_+"rho", "m",      "radius of position in the cylindrical reference frame");
        desc->AddField<double>(tableColumnNamePrefix_+"z",   "m",      "z-component of position in the cylindrical reference frame");
        desc->AddField<double>(tableColumnNamePrefix_+"phi", "radian", "phi angle of position in the cylindrical reference frame");
    }
    else if(bookRefFrame_ == I3PositionConverter::sph)
    {
        desc->AddField<double>(tableColumnNamePrefix_+"r",     "m",      "radius of position in the spherical reference frame");
        desc->AddField<double>(tableColumnNamePrefix_+"phi",   "radian", "phi angle of position in the spherical reference frame");
        desc->AddField<double>(tableColumnNamePrefix_+"theta", "radian", "theta angle of position in the spherical reference frame");
    }
    else if(bookRefFrame_ == I3PositionConverter::all)
    {
        desc->AddField<double>(tableColumnNamePrefix_+"x",     "m",      "x-component of position in the cartesian reference frame");
        desc->AddField<double>(tableColumnNamePrefix_+"y",     "m",      "y-component of position in the cartesian reference frame");
        desc->AddField<double>(tableColumnNamePrefix_+"z",     "m",      "z-component of position in the cartesian and cylindrical reference frame");
        desc->AddField<double>(tableColumnNamePrefix_+"r",     "m",      "radius of position in the spherical reference frame");
        desc->AddField<double>(tableColumnNamePrefix_+"rho",   "m",      "radius of position in the cylindrical reference frame");
        desc->AddField<double>(tableColumnNamePrefix_+"phi",   "radian", "phi angle of position in the spherical and cylindrical reference frame");
        desc->AddField<double>(tableColumnNamePrefix_+"theta", "radian", "theta angle of position in the spherical reference frame");
    }
    else {
        log_fatal("Got unknown reference frame '%d' to book!", bookRefFrame_);
    }

    return desc;
};

//______________________________________________________________________________
size_t
I3PositionConverter::
FillRows(const I3Position& position, I3TableRowPtr row)
{
    if(bookRefFrame_ == I3PositionConverter::car)
    {
        row->Set<double>(tableColumnNamePrefix_+"x", position.GetX());
        row->Set<double>(tableColumnNamePrefix_+"y", position.GetY());
        row->Set<double>(tableColumnNamePrefix_+"z", position.GetZ());
    }
    else if(bookRefFrame_ == I3PositionConverter::cyl)
    {
        row->Set<double>(tableColumnNamePrefix_+"rho", position.GetRho());
        row->Set<double>(tableColumnNamePrefix_+"z",   position.GetZ());
        row->Set<double>(tableColumnNamePrefix_+"phi", position.GetPhi());
    }
    else if(bookRefFrame_ == I3PositionConverter::sph)
    {
        row->Set<double>(tableColumnNamePrefix_+"r",     position.GetR());
        row->Set<double>(tableColumnNamePrefix_+"phi",   position.GetPhi());
        row->Set<double>(tableColumnNamePrefix_+"theta", position.GetTheta());
    }
    else if(bookRefFrame_ == I3PositionConverter::all)
    {
        row->Set<double>(tableColumnNamePrefix_+"x",     position.GetX());
        row->Set<double>(tableColumnNamePrefix_+"y",     position.GetY());
        row->Set<double>(tableColumnNamePrefix_+"z",     position.GetZ());
        row->Set<double>(tableColumnNamePrefix_+"r",     position.GetR());
        row->Set<double>(tableColumnNamePrefix_+"rho",   position.GetRho());
        row->Set<double>(tableColumnNamePrefix_+"phi",   position.GetPhi());
        row->Set<double>(tableColumnNamePrefix_+"theta", position.GetTheta());
    }
    else {
        log_fatal("Got unknown reference frame '%d' to book!", bookRefFrame_);
    }

    return 1;
};
