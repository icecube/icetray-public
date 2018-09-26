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

#include "tableio/converter/I3DirectionConverter.h"

//______________________________________________________________________________
I3TableRowDescriptionPtr
I3DirectionConverter::
CreateDescription(const I3Direction& direction)
{
    I3TableRowDescriptionPtr desc(new I3TableRowDescription());

    if(bookRefFrame_ == I3DirectionConverter::car)
    {
        desc->AddField<double>(tableColumnNamePrefix_+"x", "", "x-component of the direction as an unit vector in the cartesian reference frame");
        desc->AddField<double>(tableColumnNamePrefix_+"y", "", "y-component of the direction as an unit vector in the cartesian reference frame");
        desc->AddField<double>(tableColumnNamePrefix_+"z", "", "z-component of the direction as an unit vector in the cartesian reference frame");
    }
    else if(bookRefFrame_ == I3DirectionConverter::sph)
    {
        desc->AddField<double>(tableColumnNamePrefix_+"azimuth", "radian", "azimuth angle of the direction in the spherical reference frame");
        desc->AddField<double>(tableColumnNamePrefix_+"zenith",  "radian", "zenith angle of the direction in the spherical reference frame");
    }
    else if(bookRefFrame_ == I3DirectionConverter::all)
    {
        desc->AddField<double>(tableColumnNamePrefix_+"x",       "",       "x-component of the direction as an unit vector in the cartesian reference frame");
        desc->AddField<double>(tableColumnNamePrefix_+"y",       "",       "y-component of the direction as an unit vector in the cartesian reference frame");
        desc->AddField<double>(tableColumnNamePrefix_+"z",       "",       "z-component of the direction as an unit vector in the cartesian reference frame");
        desc->AddField<double>(tableColumnNamePrefix_+"azimuth", "radian", "azimuth angle of the direction in the spherical reference frame");
        desc->AddField<double>(tableColumnNamePrefix_+"zenith",  "radian", "zenith angle of the direction in the spherical reference frame");
    }
    else {
        log_fatal("Got unknown reference frame '%d' to book!", bookRefFrame_);
    }

    return desc;
};

//______________________________________________________________________________
size_t
I3DirectionConverter::
FillRows(const I3Direction& direction, I3TableRowPtr row)
{
    if(bookRefFrame_ == I3DirectionConverter::car)
    {
        row->Set<double>(tableColumnNamePrefix_+"x", direction.GetX());
        row->Set<double>(tableColumnNamePrefix_+"y", direction.GetY());
        row->Set<double>(tableColumnNamePrefix_+"z", direction.GetZ());
    }
    else if(bookRefFrame_ == I3DirectionConverter::sph)
    {
        row->Set<double>(tableColumnNamePrefix_+"azimuth", direction.GetAzimuth());
        row->Set<double>(tableColumnNamePrefix_+"zenith",  direction.GetZenith());
    }
    else if(bookRefFrame_ == I3DirectionConverter::all)
    {
        row->Set<double>(tableColumnNamePrefix_+"x",       direction.GetX());
        row->Set<double>(tableColumnNamePrefix_+"y",       direction.GetY());
        row->Set<double>(tableColumnNamePrefix_+"z",       direction.GetZ());
        row->Set<double>(tableColumnNamePrefix_+"azimuth", direction.GetAzimuth());
        row->Set<double>(tableColumnNamePrefix_+"zenith",  direction.GetZenith());
    }
    else {
        log_fatal("Got unknown reference frame '%d' to book!", bookRefFrame_);
    }

    return 1;
};
