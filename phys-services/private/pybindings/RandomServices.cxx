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

#include <icetray/python/context_suite.hpp>

#ifdef I3_USE_SPRNG
#include <phys-services/I3SPRNGRandomService.h>
#endif
#include <phys-services/I3GSLRandomService.h>
#include <phys-services/I3MT19937.h>

using namespace boost::python;
namespace bp = boost::python;

struct I3RandomServiceWrapper : I3RandomService, wrapper<I3RandomService>
{
  int Binomial(int ntot, double prob) { return this->get_override("Binomial")(ntot, prob); }
  double Exp(double tau) { return this->get_override("Exp")(tau); }
  unsigned int Integer(unsigned int imax) { return this->get_override("Integer")(imax); }
  int Poisson(double mean) { return this->get_override("Poisson")(mean); }
  double PoissonD(double mean) { return this->get_override("PoissonD")(mean); }
  double Uniform(double x1) { return this->get_override("Uniform")(x1); }
  double Uniform(double x1, double x2) { return this->get_override("Uniform")(x1, x2 ); }
  double Gaus(double mean, double stddev) { return this->get_override("Gaus")(mean, stddev); }
};

template <typename T, typename Init>
class_<T, boost::shared_ptr<T>, boost::python::bases<I3RandomService>, boost::noncopyable>
register_randomservice(const char* name, const char* doc, const Init& init)
{
  return class_<T, boost::shared_ptr<T>, boost::python::bases<I3RandomService>, boost::noncopyable>(name,
							     doc,
							     init)
    ;
  
}

namespace bp = boost::python;

void register_RandomServices()
{
	bp::class_<I3RandomServiceWrapper, boost::shared_ptr<I3RandomServiceWrapper>, boost::noncopyable>(
	    "I3RandomService", "Base class for random number generators", init<>())
	        .def("binomial", pure_virtual(&I3RandomService::Binomial))
	        .def("exp", pure_virtual(&I3RandomService::Exp))
	        .def("integer", pure_virtual(&I3RandomService::Integer))
	        .def("poisson", pure_virtual(&I3RandomService::Poisson))
	        .def("poisson_d", pure_virtual(&I3RandomService::PoissonD))
	        .def("uniform", pure_virtual((double (I3RandomService::*)(double)) &I3RandomService::Uniform))
	        .def("uniform", pure_virtual((double (I3RandomService::*)(double, double)) &I3RandomService::Uniform))
	        .def("gaus", pure_virtual(&I3RandomService::Gaus))
	        .add_property("state", &I3RandomService::GetState, &I3RandomService::RestoreState)
	;


  register_randomservice<I3MT19937>("I3MT19937",
				    "An implementation of the I3RandomService interface using the C++ "
				    "random number engine for MT19937",
				    init<std::vector<uint32_t> >((bp::arg("seed vector")))
				    ).def(init<>()).def(init<uint32_t>((bp::arg("seed"))));
	

  register_randomservice<I3GSLRandomService>("I3GSLRandomService", "gsl random goodness",
					     init<unsigned long int,bool>((bp::arg("seed"),bp::arg("track_state")=true)));

#ifdef I3_USE_SPRNG
  register_randomservice<I3SPRNGRandomService>("I3SPRNGRandomService", "sprng random goodness",
					       init<int, int, int,std::string,std::string>((bp::arg("seed"), 
								    bp::arg("nstreams"),
								    bp::arg("streamnum"),
								    bp::arg("instatefile")=std::string(),
								    bp::arg("outstatefile")=std::string())));
#endif
}
