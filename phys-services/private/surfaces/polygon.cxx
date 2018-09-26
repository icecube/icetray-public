/** $Id$
 * @file
 * @author Jakob van Santen <jakob.van.santen@desy.de>
 *
 * $Revision$
 * $Date$
 */

#include <phys-services/surfaces/detail/polygon.h>
#include <boost/utility.hpp>

namespace I3Surfaces { namespace polygon {

namespace {

/// A counterclockwise curve is the basic building block of a convex hull
class ccw_curve : public std::vector<vec2> {
public:
	// Add a point to the curve
	void operator()(const vec2 &p)
	{
		// Remove points until the curve will be counterclockwise
		while (size() >= 2 && !ccw((*this)[size()-2], (*this)[size()-1], p))
			pop_back();
		push_back(p);
	}
private:
	static bool
	ccw(const vec2 &o, const vec2 &a, const vec2 &b)
	{
		// 2D cross product of OA and OB vectors, i.e. z-component of their 3D cross product.
		// positive, if OAB makes a counter-clockwise turn,
		// negative for clockwise turn, and zero if the points are collinear.
		return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x) > 0;
	}
};

}

/// Lifted from http://code.icecube.wisc.edu/svn/sandbox/ckopper/eventinjector/python/util/__init__.py
std::vector<vec2>
convex_hull(const std::vector<I3Position> &positions)
{
	std::vector<vec2> hull;
	
	// Build a set of unique points, sorted lexicographically
	std::set<vec2> points;
	std::transform(positions.begin(), positions.end(),
	    std::inserter(points, points.end()), vec2::from_I3Position);
	
	// Boring case: 1 point (perhaps repeated)
	if (points.size() <= 1) {
		std::copy(points.begin(), points.end(), std::back_inserter(hull));
		return hull;
	}
	
	// Build lower and upper hulls
	std::vector<vec2> lower = std::for_each(points.begin(), points.end(), ccw_curve());
	std::vector<vec2> upper = std::for_each(points.rbegin(), points.rend(), ccw_curve());
	
	// Concatenation of the lower and upper hulls gives the convex hull.
	// Last point of each list is omitted because it is repeated at the
	// beginning of the other list.
	std::copy(lower.begin(), lower.end()-1, std::back_inserter(hull));
	std::copy(upper.begin(), upper.end()-1, std::back_inserter(hull));
	
	return hull;
}

std::vector<vec2>
expand_polygon(const std::vector<vec2> &hull, double padding)
{
	std::vector<vec2> points;
	for (std::vector<vec2>::const_iterator p = hull.begin(); p != hull.end(); p++) {
		std::vector<vec2>::const_iterator next = boost::next(p);
		if (next == hull.end())
			next = hull.begin();
		std::vector<vec2>::const_iterator prev = boost::prior(
		    p == hull.begin() ? hull.end() : p);
		// normalized vector connecting this vertex to the next one
		vec2 d = vec2::normalized(next->x-p->x, next->y-p->y);
		// and the previous vertex to this one
		vec2 prev_d = vec2::normalized(p->x-prev->x, p->y-prev->y);
		// sine of the inner angle between the segments that meet here
		double det = prev_d.x*d.y - prev_d.y*d.x;
		if (det == 0.)
			log_fatal("Edges can't be [anti]parallel");
		vec2 outwards(prev_d.x-d.x, prev_d.y-d.y);
		points.push_back(vec2(p->x + outwards.x*padding/det, p->y + outwards.y*padding/det));
	}
	
	return points;
}

vec2::vec2(double xi, double yi) : x(xi), y(yi)
{}

template <typename Archive>
void vec2::serialize(Archive &ar, unsigned version)
{
	if (version > 0)
		log_fatal_stream("Version "<<version<<" is from the future");
	
	ar & make_nvp("X", x);
	ar & make_nvp("Y", y);
}

vec2
vec2::from_I3Position(const I3Position &p)
{
	return vec2(p.GetX(), p.GetY());
}

vec2
vec2::normalized(double xi, double yi)
{
	double l = hypot(xi, yi);
	return vec2(xi/l, yi/l);
}

bool
operator<(const vec2 &a, const vec2 &b)
{
	if (a.x < b.x)
		return true;
	else if (a.x > b.x)
		return false;
	else if (a.y < b.y)
		return true;
	else
		return false;
}

side::side(const vec2 &p, const vec2 &np) : origin(p),
    vector(np.x-p.x, np.y-p.y), length(hypot(vector.x, vector.y)),
	normal(vector.y/length, -vector.x/length, 0.)
{}

}}

I3_SERIALIZABLE(I3Surfaces::polygon::vec2);
