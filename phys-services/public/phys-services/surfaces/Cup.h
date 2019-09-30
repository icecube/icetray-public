/** $Id: Cylinder.h 146654 2016-06-01 18:31:33Z cweaver $
 * @file
 * @author Jakob van Santen <jakob.van.santen@desy.de>
 *
 * $Revision: 146654 $
 * $Date: 2016-06-01 13:31:33 -0500 (Wed, 01 Jun 2016) $
 */

#ifndef I3SURFACES_CUP_H_INCLUDED
#define I3SURFACES_CUP_H_INCLUDED

#include <phys-services/surfaces/SamplingSurface.h>
#include <phys-services/surfaces/detail/CupBase.h>

namespace I3Surfaces {

/**
 * @brief A cup aligned with the z axis
 */
  class Cup : public detail::CupBase<SamplingSurface> {
  private:
    typedef detail::CupBase<SamplingSurface> Base;
  public:
    virtual ~Cup();
  Cup(double olength, double oradius, double ilength, double iradius,
      I3Position center=I3Position(0,0,0)) : Base(olength, oradius, ilength, iradius, center) {};

  private:
    Cup() {}
	
    friend class icecube::serialization::access;
    template <typename Archive>
      void serialize(Archive &, unsigned);
  };

  I3_POINTER_TYPEDEFS(Cup);

}

I3_CLASS_VERSION(I3Surfaces::Cup, 0);

#endif // I3SURFACES_CUP_H_INCLUDED
