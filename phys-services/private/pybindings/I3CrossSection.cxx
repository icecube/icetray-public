#include <phys-services/I3CrossSection.h>

using namespace boost::python;

void register_I3CrossSection()
{
  class_<I3CrossSection::finalStateRecord>("finalStateRecord",
                                           init<double, double>())
  .def_readonly("x", &I3CrossSection::finalStateRecord::x)
  .def_readonly("y", &I3CrossSection::finalStateRecord::y)
  ;

  class_<I3CrossSection>("I3CrossSection",
                         init<std::string, std::string>())
    .def("sample_final_state",
         &I3CrossSection::sampleFinalState,
         args("energy", "scatteredType", "random_service"))
    .def("evaluate_cross_section",
         &I3CrossSection::evaluateCrossSection,
         args("energy", "x", "y", "scatteredType"))
    .def("evaluate_total_cross_section",
         &I3CrossSection::evaluateTotalCrossSection,
         args("energy"))
    .def("get_min_energy",
         &I3CrossSection::GetMinimumEnergy)
    .def("get_max_energy",
         &I3CrossSection::GetMaximumEnergy)
    .def("load",
         &I3CrossSection::load,
         args("dd_crossSectionFile", "total_crossSectionFile"))
    ;
}
