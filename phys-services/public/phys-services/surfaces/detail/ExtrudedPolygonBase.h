/** $Id$
 * @file
 * @author Jakob van Santen <jakob.van.santen@desy.de>
 *
 * $Revision$
 * $Date$
 */

#ifndef I3SURFACES_EXTRUDEDPOLYGONBASE_H_INCLUDED
#define I3SURFACES_EXTRUDEDPOLYGONBASE_H_INCLUDED

#include <dataclasses/I3Direction.h>
#include <phys-services/surfaces/detail/polygon.h>
#include <phys-services/I3RandomService.h>

#include <boost/foreach.hpp>
#include <cmath>

namespace I3Surfaces {

namespace {

std::pair<double, double>
z_range(const std::vector<I3Position> &positions)
{
	std::pair<double, double> range(std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity());
	BOOST_FOREACH(const I3Position &pos, positions) {
		if (pos.GetZ() < range.first)
			range.first = pos.GetZ();
		if (pos.GetZ() > range.second)
			range.second = pos.GetZ();
	}

	return range;
}

std::pair<double, double>
make_ordered_pair(double a, double b)
{
	if (std::isnan(a) || (a > b))
		return std::make_pair(b, a);
	else
		return std::make_pair(a, b);
}

inline double
integrate_area(double a, double b, double cap, double sides)
{
	return M_PI*(cap*(b*b-a*a) +
	    sides*(acos(a) - acos(b) - sqrt(1-a*a)*a + sqrt(1-b*b)*b));
}

}

template <typename Base>
class ExtrudedPolygonBase : public Base {
public:
	ExtrudedPolygonBase(const std::vector<I3Position> &points, double padding=0.)
	{
		using namespace I3Surfaces::polygon;
	
		std::pair<double, double> zrange = z_range(points);
		std::vector<vec2> hull = convex_hull(points);
		if (padding != 0) {
			hull = expand_polygon(hull, padding);
			zrange.first -= padding;
			zrange.second += padding;
		}
		initWithHull(hull, zrange);
	}

	/// @brief Calculate distance to entry and exit points
	///
	/// The distance to the entry point is the distance from vertex `p` along
	/// direction `dir` to the point where the ray defined by `p` and `dir`
	/// enters the (convex) surface. The exit point is defined similarly.
	///
	/// @returns a pair (entry, exit). If either is NaN, then the ray does not
	/// intersect the surface.
	virtual std::pair<double, double> GetIntersection(const I3Position &p, const I3Direction &dir) const
	{
		if (dir.GetX() == 0 && dir.GetY() == 0) {
			// perfectly vertical track: only check intersections with caps
			if (!PointInHull(p))
				return Surface::no_intersection();
			else
				return GetDistanceToCaps(p, dir);
		} else if (dir.GetZ() == 0.) {
			// perfectly horizontal track: only check intersections with sides
			if (p.GetZ() < z_range_.first || p.GetZ() > z_range_.second)
				return Surface::no_intersection();
			else
				return GetDistanceToHull(p, dir);
		} else {
			// general case: both rho and z components nonzero
			std::pair<double, double> sides = GetDistanceToHull(p, dir);
			std::pair<double, double> caps = GetDistanceToCaps(p, dir);
		
			if (caps.first >= sides.second || caps.second <= sides.first)
				return Surface::no_intersection();
			else {
				return std::make_pair(std::max(sides.first, caps.first), std::min(sides.second, caps.second));
			}
		}
	}
	
	double GetArea(const I3Direction &dir) const
	{
		double area = 0;
		BOOST_FOREACH(const polygon::side &sidey, sides_) {
			double inner = sidey.normal*dir;
			if (inner < 0)
				area += -inner*sidey.length;
		}
		area *= (z_range_.second-z_range_.first);
		area += std::abs(dir.GetZ())*cap_area_;
	
		return area;
	}
	double GetMaximumArea() const
	{
		double side_area = 0;
		BOOST_FOREACH(const polygon::side &sidey, sides_) {
			side_area += sidey.length;
		}
		// the largest possible projected area occurs for a flat square
		side_area *= (z_range_.second-z_range_.first)/2.;
		double ct_max = cos(atan(side_area/cap_area_));
	
		return cap_area_*fabs(ct_max) + side_area*sqrt(1.-ct_max*ct_max);
	}
	virtual double GetAcceptance(double cosMin=0, double cosMax=1) const
	{
		double cap = GetCapArea();
		double sides = GetAverageSideArea();
		if (cosMin >= 0 && cosMax >= 0)
			return integrate_area(cosMin, cosMax, cap, sides);
		else if (cosMin < 0 && cosMax <= 0)
			return integrate_area(-cosMax, -cosMin, cap, sides);
		else if (cosMin < 0 && cosMax > 0)
			return integrate_area(0, -cosMin, cap, sides)
			    + integrate_area(0, cosMax, cap, sides);
		else
			log_fatal("Can't deal with zenith range [%.1e, %.1e]", cosMin, cosMax);
		return NAN;
	}
	
	I3Position SampleImpactPosition(const I3Direction &dir, I3RandomService &rng) const
	{
		// first, pick which face it's going to hit
		double area = 0;
		double height = GetLength();
		std::vector<double> prob;
		std::pair<double, double> x_range(std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity());
		std::pair<double, double> y_range(std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity());
	
		BOOST_FOREACH(const polygon::side &sidey, sides_) {
			double inner = sidey.normal*dir;
			if (inner < 0)
				area += -inner*sidey.length*height;
			prob.push_back(area);
			// Find the bounding box of the cap
			if (sidey.origin.x < x_range.first)
				x_range.first = sidey.origin.x;
			if (sidey.origin.x > x_range.second)
				x_range.second = sidey.origin.x;
			if (sidey.origin.y < y_range.first)
				y_range.first = sidey.origin.y;
			if (sidey.origin.y > y_range.second)
				y_range.second = sidey.origin.y;
		}
		area += std::abs(dir.GetZ())*cap_area_;
		prob.push_back(area);
		std::vector<double>::iterator target =
		    std::lower_bound(prob.begin(), prob.end(), rng.Uniform(0, area));
		if (target == boost::prior(prob.end())) {
			// top or bottom face
			// triangulation would be more efficient here, but also more complicated
			I3Position pos(NAN,NAN,dir.GetZ() > 0 ? z_range_.first : z_range_.second);
			do {
				pos.SetX(rng.Uniform(x_range.first, x_range.second));
				pos.SetY(rng.Uniform(y_range.first, y_range.second));
			} while (!PointInHull(pos));
			return pos;
		} else {
			// side face
			std::vector<polygon::side>::const_iterator sidey =
			    sides_.begin() + std::distance(prob.begin(), target);
			double horizontal = rng.Uniform();
			double vertical = rng.Uniform();
			return I3Position(
			    sidey->origin.x + horizontal*sidey->vector.x,
			    sidey->origin.y + horizontal*sidey->vector.y,
			    z_range_.first + vertical*height
			);
		}
	}
	
	std::vector<double> GetX() const
	{
		std::vector<double> x;
		BOOST_FOREACH(const polygon::side &sidey, sides_)
			x.push_back(sidey.origin.x);
		return x;
	}
	std::vector<double> GetY() const
	{
		std::vector<double> y;
		BOOST_FOREACH(const polygon::side &sidey, sides_)
			y.push_back(sidey.origin.y);
		return y;
	}
	std::vector<double> GetZ() const
	{
		std::vector<double> z;
		z.push_back(z_range_.first);
		z.push_back(z_range_.second);
		return z;
	}

protected:
	ExtrudedPolygonBase() {};
	double GetCapArea() const { return cap_area_; }
	double GetAverageSideArea() const
	{
		// the projected area of a plane, averaged over a 2\pi rotation that
		// passes through the normal, is
		// A*\int_0^\pi \Theta(\sin\alpha)\sin\alpha d\alpha / 2\pi = A/\pi
		double area = 0;
		BOOST_FOREACH(const polygon::side &sidey, sides_)
			area += sidey.length;
		area *= GetLength()/M_PI;
	
		return area;
	}
	double GetLength() const { return z_range_.second-z_range_.first; }
	std::pair<double, double> GetZRange() const { return z_range_; }

private:
	std::vector<polygon::side> sides_;
	std::pair<double, double> z_range_;
	double cap_area_;
	
	void initWithHull(const std::vector<polygon::vec2> &hull, const std::pair<double,double> &zrange)
	{
		using I3Surfaces::polygon::vec2;
		using I3Surfaces::polygon::side;
	
		z_range_ = zrange;
		cap_area_ = 0;
		sides_.clear();
		for (std::vector<vec2>::const_iterator p = hull.begin(); p != hull.end(); p++) {
			std::vector<vec2>::const_iterator np = boost::next(p);
			if (np == hull.end())
				np = hull.begin();
		
			sides_.push_back(side(*p, *np));
		
			// area of a simple polygon in the x-y plane
			cap_area_ += (p->x*np->y - np->x*p->y);
		}
		cap_area_ /= 2.;
	}
	
	/// @brief Get distances to the infinite horizontal planes that define the 
	///        top and bottom of the surface
	/// 
	/// The distance to the entry point is the distance from vertex `p` along
	/// direction `dir` to the point where the ray defined by `p` and `dir`
	/// crosses the top or the bottom plane, whichever comes first. The exit 
	/// point is defined similary.
	std::pair<double, double> GetDistanceToCaps(const I3Position &p, const I3Direction &dir) const
	{
		return make_ordered_pair(GetDistanceToCap(p, dir, z_range_.first), GetDistanceToCap(p, dir, z_range_.second));
	}
	
	/// @brief Get distances to sides of the of the surface
	///
	/// The sides of the surface are defined by a polygon in the x-y plane,
	/// extended infinitely along the z axis. The distance to the entry point 
	/// is the distance from vertex `p` along direction `dir` to the point where
	/// the ray defined by `p` and `dir` first crosses one of the line
	/// segments that define the x-y polygon. The exit point is the distance to
	/// the last such crossing.
	std::pair<double, double> GetDistanceToHull(const I3Position &pos, const I3Direction &dir) const
	{
		std::pair<double, double> offsets = Surface::no_intersection();
	
		if (dir.GetX() == 0 && dir.GetY() == 0)
			log_fatal("Direction must have a horizontal component");
	
		BOOST_FOREACH(const polygon::side &sidey, sides_) {
			// Components of the vector connecting the test
			// point to the origin of the line segment
			double x = sidey.origin.x - pos.GetX();
			double y = sidey.origin.y - pos.GetY();
		
			// Proportional distance along the line segment to the
			// intersection point
			double alpha = (dir.GetX()*y - dir.GetY()*x)
			    / (dir.GetY()*sidey.vector.x - dir.GetX()*sidey.vector.y);
		
			// Is there an intersection?
			if ((alpha >= 0.) && (alpha < 1.)) {
				// Distance along ray to intersection point
				double beta = dir.GetX() != 0. ?
				    (x + alpha*sidey.vector.x)/dir.GetX() :
				    (y + alpha*sidey.vector.y)/dir.GetY();
				// NB: reversed comparison is equivalent to
				// (isnan(offsets.first) || beta < offsets.first)
				if (!(beta >= offsets.first))
					offsets.first = beta;
				if (!(beta <= offsets.second))
					offsets.second = beta;
			}
		}
	
		return offsets;
	}

	/// @brief Test whether `pos` is in the x-y polygon
	bool PointInHull(const I3Position &pos) const
	{
		// Test for containment by ray casting.
		int crossings = 0;
		for (std::vector<polygon::side>::const_iterator p = sides_.begin(); p != sides_.end(); p++) {
			std::vector<polygon::side>::const_iterator np = boost::next(p);
			if (np == sides_.end())
				np = sides_.begin();
		
			// only consider segments whose y range spans the current point
			if (!(((p->origin.y > pos.GetY()) && (np->origin.y <= pos.GetY())) ||
			      ((p->origin.y <= pos.GetY()) && (np->origin.y > pos.GetY()))))
				continue;
			// find the intersection of the segment with a horizontal line
			double xc = p->origin.x + (pos.GetY() - p->origin.y)*p->vector.x/p->vector.y;
			// is the crossing to the right of the test point?
			if (pos.GetX() < xc)
				crossings++;
		}
	
		// the point is inside iff the ray crosses an odd number of times
		return (crossings % 2) == 1;
	}
	double GetDistanceToCap(const I3Position &p, const I3Direction &dir, double cap_z) const
	{
		return (cap_z-p.GetZ())/dir.GetZ();
	}
	
	friend class icecube::serialization::access;
	template <typename Archive>
	void save(Archive &ar, unsigned version) const
	{
		if (version > 0)
			log_fatal_stream("Version "<<version<<" is from the future");

		ar & make_nvp("Base", base_object<Base>(*this));
		std::vector<polygon::vec2> hull;
		BOOST_FOREACH(const polygon::side &sidey, sides_) {
			hull.push_back(sidey.origin);
		}
		ar & make_nvp("HullXY", hull);
		ar & make_nvp("HullZ", z_range_);
	}
	template <typename Archive>
	void load(Archive &ar, unsigned version)
	{
		if (version > 0)
			log_fatal_stream("Version "<<version<<" is from the future");

		ar & make_nvp("Base", base_object<Base>(*this));
		std::vector<polygon::vec2> hull;
		std::pair<double, double> z_range;
		ar & make_nvp("HullXY", hull);
		ar & make_nvp("HullZ", z_range);
		initWithHull(hull, z_range);
	}
	I3_SERIALIZATION_SPLIT_MEMBER();
};

}

#endif // I3SURFACES_EXTRUDEDPOLYGONBASE_H_INCLUDED
