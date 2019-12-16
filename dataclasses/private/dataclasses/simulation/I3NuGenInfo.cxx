#include <icetray/serialization.h>
#include <dataclasses/simulation/I3NuGenInfo.h>

template <class Archive>
void 
I3NuGenInfo::serialize(Archive& ar, unsigned version)
{
  if (version>i3nugen_info_version_){
    log_fatal("Attempting to read version %u from file but running version %u of I3NuGenInfo class.",version,i3nugen_info_version_);
  }
  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("run_id", run_id);
  ar & make_nvp("n_events",n_events);  
  ar & make_nvp("primary_type",primary_type);
  ar & make_nvp("flavor_ratio",flavor_ratio);  
  ar & make_nvp("cylinder_height",cylinder_height);
  ar & make_nvp("cylinder_radius",cylinder_radius);
  ar & make_nvp("min_zenith",min_zenith);
  ar & make_nvp("min_azimuth",min_azimuth);
  ar & make_nvp("max_zenith",max_zenith);  
  ar & make_nvp("max_azimuth", max_azimuth);
  ar & make_nvp("min_energy_log",min_energy_log);  
  ar & make_nvp("max_energy_log",max_energy_log);
  ar & make_nvp("power_law_index",power_law_index);
}

std::ostream& I3NuGenInfo::Print(std::ostream& oss) const{  
  oss << "[ I3NuGenInfo" << std::endl
      << "            run_id : " << run_id << std::endl
      << "          n_events : " << n_events << std::endl
      << "      primary_type : " << primary_type << std::endl
      << "      flavor_ratio : " << flavor_ratio << std::endl    
      << "   cylinder_height : " << cylinder_height << std::endl
      << "   cylinder_radius : " << cylinder_radius << std::endl
      << "        min_zenith : " << min_zenith << std::endl
      << "        max_zenith : " << max_zenith << std::endl
      << "       min_azimuth : " << min_azimuth << std::endl    
      << "       max_azimuth : " << max_azimuth << std::endl
      << "    min_energy_log : " << min_energy_log << std::endl    
      << "    max_energy_log : " << max_energy_log << std::endl
      << "   power_law_index : " << power_law_index << std::endl
      << "]" ;
  return oss;
}

std::ostream& operator<<(std::ostream& oss, const I3NuGenInfo& n){
  return(n.Print(oss));
}

I3_SERIALIZABLE(I3NuGenInfo);
