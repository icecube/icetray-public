/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Fabian Kislat <fabian.kislat@desy.de> $LastChangedBy$
 */

#ifndef TABLEIO_DATACLASSES_CONTAINER_CONVERT_H_INCLUDED
#define TABLEIO_DATACLASSES_CONTAINER_CONVERT_H_INCLUDED

#include <icetray/I3PointerTypedefs.h>
#include <icetray/OMKey.h>
#include <dataclasses/TankKey.h>
#include <dataclasses/physics/I3Trigger.h>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <dataclasses/physics/I3RecoHit.h>
#include <dataclasses/physics/I3RecoPulse.h>
#include <dataclasses/physics/I3MCHit.h>
#include <dataclasses/physics/I3FlasherInfo.h>
#include <tableio/I3TableRowDescription.h>
#include <tableio/I3TableRow.h>
#include <utility>

#include "pod_converter_type_mapping.h"

#define TABLEIO_CONVERTER_FWD_BODY \
         void AddFields(I3TableRowDescriptionPtr desc, const booked_type& = booked_type()); \
         void FillSingleRow(const booked_type& hit, I3TableRowPtr row)

#define TABLEIO_CONVERTER_FWD(type)     \
         struct type                    \
         {                              \
           typedef ::type booked_type;  \
           TABLEIO_CONVERTER_FWD_BODY;  \
         }

/// @cond
namespace convert {

  template <typename T>
  struct pod {
    typedef T booked_type;

    void AddFields(I3TableRowDescriptionPtr desc, const booked_type& = booked_type())
    {
      desc->AddField< typename detail::pod_converter_type_mapping<T>::type >("item", "", "Item in the booked vector");
    }

    void FillSingleRow(const booked_type &item, I3TableRowPtr row)
    {
      row->Set< typename detail::pod_converter_type_mapping<T>::type >("item", item);
    }
  };

  TABLEIO_CONVERTER_FWD(I3Trigger);
  TABLEIO_CONVERTER_FWD(I3DOMLaunch);
  TABLEIO_CONVERTER_FWD(I3RecoHit);
  TABLEIO_CONVERTER_FWD(I3MCHit);
  TABLEIO_CONVERTER_FWD(I3RecoPulse);
  TABLEIO_CONVERTER_FWD(I3FlasherInfo);
  TABLEIO_CONVERTER_FWD(OMKey);
  TABLEIO_CONVERTER_FWD(TankKey);

  struct double_pair
  {
    typedef ::std::pair<double, double> booked_type;\
    TABLEIO_CONVERTER_FWD_BODY;
  };
  

}
/// @endcond

#endif // TABLEIO_DATACLASSES_CONTAINER_CONVERT_H_INCLUDED
