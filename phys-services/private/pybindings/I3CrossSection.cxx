/**
 *
 * Implementation of Python bindings for I3CrossSection class
 *
 * (c) 2018
 * the IceCube Collaboration
 * $Id$
 *
 * @file I3CrossSection.cxx
 * @date $Date$
 * @author mmeier
 *
 */
#include "phys-services/I3CrossSection.h"

#include <string>

#include <boost/noncopyable.hpp>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

namespace bp = boost::python;

void register_I3CrossSection()
{
    bp::class_<I3CrossSection::finalStateRecord>
    (
        "finalStateRecord",
        bp::init<double, double>()
    )
    .def_readonly("x", &I3CrossSection::finalStateRecord::x)
    .def_readonly("y", &I3CrossSection::finalStateRecord::y)
    ;

    bp::class_<I3CrossSection, boost::noncopyable>
    (
        "I3CrossSection",
        bp::init<const std::string&, const std::string&>()
    )
    .def(
        "sample_final_state",
        &I3CrossSection::sampleFinalState,
        bp::args("energy", "scatteredType", "random_service")
    )
    .def(
        "evaluate_cross_section",
        &I3CrossSection::evaluateCrossSection,
        bp::args("energy", "x", "y", "scatteredType")
    )
    .def(
        "evaluate_total_cross_section",
        &I3CrossSection::evaluateTotalCrossSection,
        bp::args("energy")
    )
    .def(
        "get_min_energy",
        &I3CrossSection::getMinimumEnergy
    )
    .def(
        "get_max_energy",
        &I3CrossSection::getMaximumEnergy
    )
    .def(
        "load",
        &I3CrossSection::load,
        bp::args("dd_crossSectionFile", "total_crossSectionFile")
    )
    ;
}
