/** $Id$
 * @file
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 * $Revision$
 * $Date$
 */

#include <phys-services/surfaces/Cylinder.h>
#include <phys-services/surfaces/Cup.h>
#include <phys-services/surfaces/ExtrudedPolygon.h>
#include <phys-services/surfaces/AxialCylinder.h>
#include <phys-services/surfaces/Sphere.h>

#include <dataclasses/I3Position.h>
#include <dataclasses/I3Direction.h>
#include <phys-services/I3RandomService.h>
#include <icetray/python/dataclass_suite.hpp>
#include <icetray/python/gil_holder.hpp>

static boost::python::tuple SampleImpactRay(const I3Surfaces::SamplingSurface &s, I3RandomServicePtr rng, double cosMin, double cosMax)
{
	I3Position pos;
	I3Direction dir;
	s.SampleImpactRay(pos, dir, *rng, cosMin, cosMax);
	return boost::python::make_tuple(pos, dir);
}

namespace I3Surfaces {

using namespace boost::python;

class PySurface : public Surface, public wrapper<Surface> {
public:
	virtual std::pair<double, double> GetIntersection(const I3Position &p, const I3Direction &dir) const
	{
		boost::python::detail::gil_holder lock;
		return get_override("GetIntersection")(p, dir);
	}
	
	virtual bool operator==(const Surface &s) const
	{
		log_fatal("Python Surfaces can't be compared.");
		return false;
	}
};

I3_POINTER_TYPEDEFS(PySurface);

}

void register_Surface()
{
	using namespace I3Surfaces;
	using namespace boost::python;
	
	class_<PySurface, PySurfacePtr, boost::noncopyable>("Surface")
	    .def("intersection", &Surface::GetIntersection)
	;
	
	implicitly_convertible<SurfacePtr, SurfaceConstPtr>();
	
	class_<SamplingSurface, SamplingSurfacePtr, bases<Surface>, boost::noncopyable>("SamplingSurface", no_init)
	    .def("area", &SamplingSurface::GetArea, (arg("dir")))
	    .def("maximum_area", &SamplingSurface::GetMaximumArea)
	    .def("sample_impact_ray", &SampleImpactRay, (arg("self"), arg("rng"), arg("cosMin")=0, arg("cosMax")=1))
	    .def("sample_impact_position", &SamplingSurface::SampleImpactPosition, (arg("self"), arg("dir"), arg("rng")))
            .def("acceptance", &SamplingSurface::GetAcceptance, (arg("cosMin")=0., arg("cosMax")=1.))
	;
	
	implicitly_convertible<SamplingSurfacePtr, SamplingSurfaceConstPtr>();
	
	class_<Cylinder, CylinderPtr, bases<SamplingSurface> >("Cylinder",
	    init<double, double, I3Position>((arg("length"), arg("radius"), arg("center")=I3Position(0,0,0))))
	    .def(copy_suite<Cylinder>())
	    .def(dataclass_suite<Cylinder>())
	    #define PROPS (Length)(Radius)(Center)
	    BOOST_PP_SEQ_FOR_EACH(WRAP_PROP, Cylinder, PROPS)
	    #undef PROPS
	;
	
	implicitly_convertible<CylinderPtr, CylinderConstPtr>();
	
	class_<Cup, CupPtr, bases<SamplingSurface> >("Cup",
            init<double, double, double, double, I3Position>((arg("olength"), arg("oradius"), arg("ilength"), arg("iradius"), arg("center")=I3Position(0,0,0))))
	    .def(copy_suite<Cup>())
	    #define PROPS (Length)(Radius)(Center)
	    BOOST_PP_SEQ_FOR_EACH(WRAP_PROP, Cup, PROPS)
	    #undef PROPS
	;
	
	implicitly_convertible<CupPtr, CupConstPtr>();

	class_<ExtrudedPolygon, ExtrudedPolygonPtr, bases<SamplingSurface> >("ExtrudedPolygon",
	    init<const std::vector<I3Position> &, double>((arg("points"), arg("padding")=0)))
	    .add_property("x", &ExtrudedPolygon::GetX)
	    .add_property("y", &ExtrudedPolygon::GetY)
	    .add_property("z", &ExtrudedPolygon::GetZ)
	;
	
	class_<Sphere, bases<Surface> >("Sphere", init<double, double>())
	;
	
	class_<AxialCylinder, bases<SamplingSurface> >("AxialCylinder",
	    init<double,double,I3Position>((bp::arg("length"), bp::arg("radius"), bp::arg("center")=I3Position(0,0,0))))
	    .def(init<double,double,double,I3Position>((bp::arg("lengthBefore"), "lengthAfter", "radius", bp::arg("center")=I3Position(0,0,0))))
          .def(dataclass_suite<AxialCylinder>())
	;
}
