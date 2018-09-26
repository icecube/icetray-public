//
//   Copyright (c) 2008   Troy D. Straszheim and the IceCube Collaboration 
//   
//   This file is part of IceTray.
//
//   IceTray is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
//   IceTray is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

using namespace boost::python;
namespace bp = boost::python;

#include <phys-services/I3Calculator.h>

static bp::tuple
GetTransverseDirections(const I3Direction &dir)
{
    std::pair<I3Direction,I3Direction> perp =
        I3Calculator::GetTransverseDirections(dir);
    return bp::make_tuple(perp.first, perp.second);
}

void register_I3Calculator()
{
  // map the I3Calculator namespace to a sub-module  
  // make "from phys-services import <whatever>" work  
  bp::object I3CalculatorModule(bp::handle<>(bp::borrowed(PyImport_AddModule("phys-services.I3Calculator"))));  
  // make "from phys-services import I3Calculator" work  
  bp::scope().attr("I3Calculator") = I3CalculatorModule;  
  // set the current scope to the new sub-module  
  bp::scope I3Calculator_scope = I3CalculatorModule;  
  // export stuff in the I3Calculator namespace  
  def("closest_approach_distance", I3Calculator::ClosestApproachDistance,
      "Calculate the minimum perpendicular distance between a track and a point",
      (bp::arg("particle"), bp::arg("position")));
  def("closest_approach_position", I3Calculator::ClosestApproachPosition,
      "Calculate the position along a track with the minimum perpendicular distance to a point",
      (bp::arg("particle"), bp::arg("position")));
  def("distance_along_track", I3Calculator::DistanceAlongTrack,
      "Calculate the distance (projected onto the track) between the track start position and a point",
      (bp::arg("particle"), bp::arg("position")));
  def("is_on_track", I3Calculator::IsOnTrack,
      "Check if the given point is on the given track to within a tolerance",
      (bp::arg("particle"), bp::arg("position"), bp::arg("tol")=10*I3Units::cm));
  def("time_residual", (double (*)( const I3Particle&, const I3Position&,  double, const double, const double)) &I3Calculator::TimeResidual,
      "Calculate the difference between the given time and the earliest possible Cherenkov photon arrival time for a given track at a given position",
      (bp::arg("particle"), bp::arg("position"), bp::arg("time"),
       bp::arg("n_group")=I3Constants::n_ice_group, bp::arg("n_phase")=I3Constants::n_ice_phase));
  def("angle", I3Calculator::Angle,
      "Calculate the opening angle between two particle directions");
  def("distance", I3Calculator::Distance,
      "Calculate the minimum 3-distance between two particles");
  def("four_distance", I3Calculator::FourDistance,
      "Calculate the minimum 4-distance between two particles");
  def("rotate", I3Calculator::Rotate,
      "Rotate a direction around the given axis",
      (bp::arg("axis"), bp::arg("direction"), bp::arg("angle")));
  def("reverse_direction", I3Calculator::GetReverseDirection,
      "Rotate a direction around the given axis");
  def("transverse_directions", GetTransverseDirections,
      "Return the directions transverse to the given direction");
  def("in_shower_system",
      (I3Position (*)( const I3Position&, const I3Direction&, const I3Position&))
        &I3Calculator::InShowerSystem,
      "Return the position in the shower coordinate system (SCS).");
  def("cherenkov_position",I3Calculator::CherenkovPosition,
      "Obtain the Cherenkov position",
      (bp::arg("particle"), bp::arg("position"),
       bp::arg("n_group")=I3Constants::n_ice_group, bp::arg("n_phase")=I3Constants::n_ice_phase));
  def("cherenkov_time", I3Calculator::CherenkovTime,
      "Obtain the Cherenkov time",
      (bp::arg("particle"), bp::arg("position"), 
       bp::arg("n_group")=I3Constants::n_ice_group, bp::arg("n_phase")=I3Constants::n_ice_phase));
  def("cherenkov_distance", I3Calculator::CherenkovDistance,
      "Obtain the Cherenkov distance traveled by a photon",
      (bp::arg("particle"), bp::arg("position"), 
       bp::arg("n_group")=I3Constants::n_ice_group, bp::arg("n_phase")=I3Constants::n_ice_phase));
  def("cherenkov_approach_angle",I3Calculator::CherenkovApproachAngle,
      "Obtain the Cherenkov approach angle",
      (bp::arg("track"), bp::arg("position"), bp::arg("direction"),
       bp::arg("n_group")=I3Constants::n_ice_group, bp::arg("n_phase")=I3Constants::n_ice_phase));  
}
