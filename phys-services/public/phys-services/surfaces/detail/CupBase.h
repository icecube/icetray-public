/** $Id$
 * @file
 * @author Jakob van Santen <jakob.van.santen@desy.de>
 *
 * $Revision$
 * $Date$
 */

#ifndef I3SURFACES_CUPBASE_H_INCLUDED
#define I3SURFACES_CUPBASE_H_INCLUDED

#include <dataclasses/I3Position.h>
#include <dataclasses/I3Direction.h>
#include <phys-services/I3RandomService.h>

namespace cupspace {

  inline void
    sort(std::pair<double, double> &pair)
  {
    if (pair.first > pair.second) {
      double aux = pair.first;
      pair.first = pair.second;
      pair.second = aux;
    }
  }

  inline double
    integrate_area(double a, double b, double ocap, double osides, double icap, double isides)
  {
    return M_PI*((ocap-icap)*(b*b-a*a) +
                 (osides-isides)*(acos(a) - acos(b) - sqrt(1-a*a)*a + sqrt(1-b*b)*b));
  }

}

namespace I3Surfaces { namespace detail {

    template <typename Base>
      class CupBase : public Base {
    public:
    CupBase(double olength, double ilength, double oradius, double iradius, I3Position center=I3Position(0,0,0))
      : olength_(ilength), oradius_(oradius), ilength_(ilength), iradius_(iradius), center_(center)
      {}

      // I3Surfaces::Surface interface
      // Find the distances to the points of intersection with a cylinder 
      // aligned along the z axis. Adapted from:
      // http://code.icecube.wisc.edu/svn/projects/mmc/trunk/src/tfa/Amanda.java
      // (D. Chirkin)
      std::pair<double, double> GetIntersection(const I3Position &p, const I3Direction &dir) const
      {
        std::pair<double, double> h(Surface::no_intersection()), r(Surface::no_intersection());
	
        double x = p.GetX()-center_.GetX();
        double y = p.GetY()-center_.GetY();
        double z = p.GetZ()-center_.GetZ();
	
        double sinph = sin(dir.GetAzimuth());
        double cosph = cos(dir.GetAzimuth());
        double sinth = sin(dir.GetZenith());
        double costh = cos(dir.GetZenith());
	
        double b = x*cosph + y*sinph;
        double d = b*b + oradius_*oradius_ - x*x - y*y;
	
        if (d > 0) {
          d = sqrt(d);
          // down-track distance to the endcaps
          if (costh != 0) {
            h.first  = (z - olength_/2)/costh;
            h.second = (z + olength_/2)/costh;
            cupspace::sort(h);
          }
          // down-track distance to the side surfaces
          if (sinth != 0) {
            r.first  = (b - d)/sinth;
            r.second = (b + d)/sinth;
            cupspace::sort(r);
          }
          // Perfectly horizontal tracks never intersect the endcaps
          if (costh == 0) {
            if ((z > -olength_/2) && (z < olength_/2))
              h = r;
            else
              h = Surface::no_intersection();
            // Perfectly vertical tracks never intersect the osides
          } else if (sinth == 0) {
            if (hypot(x, y) >= oradius_)
              h = Surface::no_intersection();
            // For general tracks, take the last entrace and first exit
          } else {
            if (h.first >= r.second || h.second <= r.first)
              h = Surface::no_intersection();
            else {
              h.first = std::max(r.first, h.first);
              h.second = std::min(r.second, h.second);
            }
          }
        }
	
        return h;
      }

      // I3Surfaces::SamplingSurface interfact
      double GetArea(const I3Direction &dir) const
      {
        return GetAreaForZenith(-dir.GetZ());
      }
      double GetMaximumArea() const
      {
        double thetaMax = atan(2*olength_/(M_PI*oradius_));
        return GetAreaForZenith(cos(thetaMax));
      }
      virtual double GetAcceptance(double cosMin=0, double cosMax=1) const
      {
        double ocap = M_PI*oradius_*oradius_;
        double osides = 2*oradius_*olength_;
        double icap = M_PI*iradius_*iradius_;
        double isides = 2*iradius_*ilength_;
        if (cosMin >= 0 && cosMax >= 0)
          return cupspace::integrate_area(cosMin, cosMax, ocap, osides, icap, isides);
        else if (cosMin < 0 && cosMax <= 0)
          return cupspace::integrate_area(-cosMax, -cosMin, ocap, osides, icap, isides);
        else if (cosMin < 0 && cosMax > 0)
          return cupspace::integrate_area(0, -cosMin, ocap, osides, icap, isides)
            + cupspace::integrate_area(0, cosMax, ocap, osides, icap, isides);
        else
          log_fatal("Can't deal with zenith range [%.1e, %.1e]", cosMin, cosMax);
        return NAN;
      }
	
      // SamplingSurface interface
      I3Direction SampleDirection(I3RandomService &rng, double cosMin=0, double cosMax=1) const
      {
        // Sample a direction proportional to the projected area 
        // of the surface.
        double coszen;
        double maxarea = GetMaximumArea();
        do {
          coszen = rng.Uniform(cosMin, cosMax);
        } while (rng.Uniform(0, maxarea) > GetAreaForZenith(coszen));
	
        return I3Direction(acos(coszen), rng.Uniform(0, 2*M_PI));
      }
      I3Position SampleImpactPosition(const I3Direction &dir, I3RandomService &rng) const
      {
        // The projection of a cylinder onto a plane whose
        // normal is inclined by `zenith` w.r.t to the
        // cylinder axis is a rectangle of width 2*r and
        // height h*sin(theta) capped with two half-ellipses
        // of major axis r and minor axis r*cos(theta). The
        // projection of a cup, exclusive of the inner region,
        // is the difference of two cylindric projects. Pick a
        // point from a uniform distribution over this area.
        double ia = sin(dir.GetZenith())*ilength_/2.;
        double ib = fabs(cos(dir.GetZenith()))*iradius_;
        double oa = sin(dir.GetZenith())*olength_/2.;
        double ob = fabs(cos(dir.GetZenith()))*oradius_;
        double coff = sin(dir.GetZenith())*(olength_-ilength_)/2;
        double x, y;
        do {
          x = oradius_*rng.Uniform(-1, 1);
          y = (oa + ob)*rng.Uniform(-1, 1);
        } while (fabs(y) > oa + ob*sqrt(1 - (x*x)/(oradius_*oradius_)) ||
                 (fabs(x) < iradius_ && fabs(y-coff) < ia+ib*sqrt(1-(x*x)/(iradius_*iradius_))));
        // Rotate into the transverse plane
        I3Position impact(y, x, 0);
        impact.RotateY(dir.GetZenith());
        impact.RotateZ(dir.GetAzimuth());
        // Shift from cylinder-centered to real coordinates
        impact.SetX(impact.GetX() + center_.GetX());
        impact.SetY(impact.GetY() + center_.GetY());
        impact.SetZ(impact.GetZ() + center_.GetZ());
        // Project back to the entry point
        double l = GetIntersection(impact, dir).first;
        impact.SetX(impact.GetX() + l*dir.GetX());
        impact.SetY(impact.GetY() + l*dir.GetY());
        impact.SetZ(impact.GetZ() + l*dir.GetZ());
	
        return impact;
      }
      void SetLength(double v) { olength_ = v; }
      double GetLength() const { return olength_; }
	
      void SetRadius(double v) { oradius_ = v; }
      double GetRadius() const { return oradius_; }
	
      void SetCenter(const I3Position &v) { center_ = v; }
      I3Position GetCenter() const { return center_; }

    protected:
      CupBase() {}

    private:
      double GetAreaForZenith(double coszen) const
      {
        double ocap = M_PI*oradius_*oradius_;
        double osides = 2*oradius_*olength_;
        double icap = M_PI*iradius_*iradius_;
        double isides = 2*iradius_*ilength_;
        return (ocap-icap)*fabs(coszen) + (osides-isides)*sqrt(1.-coszen*coszen);
      }

      friend class icecube::serialization::access;
      template <typename Archive>
	void serialize(Archive &ar, unsigned version)
      {
        if (version > 0)
          log_fatal_stream("Version "<<version<<" is from the future");
	
        ar & make_nvp("Base", base_object<Base>(*this));
        ar & make_nvp("Outer Length", olength_);
        ar & make_nvp("Outer Radius", oradius_);
        ar & make_nvp("Inner Length", ilength_);
        ar & make_nvp("Inner Radius", iradius_);
        ar & make_nvp("Center", center_);
      }

      double olength_, oradius_, ilength_, iradius_;
      I3Position center_;
    };

  }}

#endif // I3SURFACES_CUPBASE_H_INCLUDED
