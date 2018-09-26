/** $Id$
 * @file
 * @author Jakob van Santen <jakob.van.santen@desy.de>
 *
 * $Revision$
 * $Date$
 */

#ifndef I3SURFACES_POLYGON_H_INCLUDED
#define I3SURFACES_POLYGON_H_INCLUDED

#include <dataclasses/I3Direction.h>

class I3Position;

namespace I3Surfaces { namespace polygon {

struct vec2 {
	vec2(double xi, double yi);
	static vec2 from_I3Position(const I3Position &pos);
	static vec2 normalized(double xi, double yi);

	double x, y;

private:
	vec2() {};
	friend class icecube::serialization::access;
	template <typename Archive>
	void serialize(Archive &, unsigned);
};

struct side {
	side(const vec2 &point, const vec2 &next_point);
	vec2 origin;
	vec2 vector;
	double length;
	I3Direction normal;
};

/// @brief Computes the convex hull of a set of 2D points.
/// Input: an iterable sequence of (x, y) pairs representing the points.
/// Output: a list of vertices of the convex hull in counter-clockwise order,
///   starting from the vertex with the lexicographically smallest coordinates.
/// Implements Andrew's monotone chain algorithm. O(n log n) complexity.
std::vector<vec2> convex_hull(const std::vector<I3Position> &positions);

/// @brief Expand the x-y footprint by moving each edge out by a distance
/// *padding*.
/// 
/// A convex polygon can be offset by moving each vertex parallel to the
/// edges by a distance that is inversely proportional to the sine of the
/// counterclockwise angle between the edges that meet at each vertex.
/// This breaks down for edges that are [anti]parallel or, but neither
/// case should occur for maximally simplified polygons.
std::vector<vec2> expand_polygon(const std::vector<vec2> &hull, double padding);

}}

#endif // I3SURFACES_POLYGON_H_INCLUDED
