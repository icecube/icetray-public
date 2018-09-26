#include <phys-services/I3VEMCalManipulator.h>
#include <fstream>
#include <math.h>
#include <dirent.h>

I3VEMCalManipulator::I3VEMCalManipulator(I3CalibrationService& calService):
calService_(calService)
{
    currentCalib_ = I3CalibrationPtr();
    filepath_   = "";
    forceFile_  = false;
    useDefaults_ = false;
}


I3VEMCalManipulator::~I3VEMCalManipulator()
{

}


I3CalibrationConstPtr I3VEMCalManipulator::GetCalibration(I3Time time)
{
    if(currentCalib_)
    {
	if(time>=currentCalib_->startTime && time<currentCalib_->endTime)
	{
	    return currentCalib_;
	}
    }
    
    I3CalibrationConstPtr calib = calService_.GetCalibration(time);
    if(!calib) return I3CalibrationConstPtr();
    
    currentCalib_ = I3CalibrationPtr(new I3Calibration(*calib));
    SetVEMCalibration(*currentCalib_, time);
    
    CheckVEMCalibration(*currentCalib_);
    
    return currentCalib_;
}


void I3VEMCalManipulator::SetVEMCalibration(I3Calibration& calib, const I3Time& time)
{
    calib.vemCal.clear();
    
    I3VEMCalList::const_iterator vem_iter;
    I3VEMCalList::const_iterator before_iter = vemCalList_.end();
    I3VEMCalList::const_iterator after_iter  = vemCalList_.end();
    
    bool forced=false;
    if(!filepath_.empty() && vemCalList_.size()==1)
    {
	// Single file mode
	vem_iter = vemCalList_.begin();
	if(vem_iter->first < time)
	{
	    before_iter = vem_iter;
	}
	
	if(forceFile_)
	{
	    before_iter = vem_iter;
	    forced = true;
	}
    }
    else
    {
	// Directory mode
	for(vem_iter = vemCalList_.begin(); vem_iter != vemCalList_.end(); vem_iter++)
	{
	    // Only use VEM calibration in the calibration time range
	    if(vem_iter->first<calib.startTime || vem_iter->first>=calib.endTime) continue;
	    
	    // Find the first VEM calibration before time
	    if(vem_iter->first < time)
	    {
		if(before_iter == vemCalList_.end())
		{
		    before_iter = vem_iter; // Initialization
		}
		else
		{
		    if(vem_iter->first > before_iter->first) before_iter = vem_iter;
		}
	    }
	    
	    // Find the first VEM calibration after time
	    if(vem_iter->first > time)
	    {
		if(after_iter == vemCalList_.end())
		{
		    after_iter = vem_iter; // Initialization
		}
		else
		{
		    if(vem_iter->first < after_iter->first) after_iter = vem_iter;
		}
	    }
	}
    }
	
    if(before_iter!=vemCalList_.end())
    {
	calib.vemCal = before_iter->second;
	if(!forced)                       calib.startTime = before_iter->first;
	if(after_iter!=vemCalList_.end()) calib.endTime   = after_iter->first;
	
	log_info("Using VEM calibration from %s.", before_iter->first.GetUTCString().c_str());
    }    
}


void I3VEMCalManipulator::CheckVEMCalibration(I3Calibration& calib)
{
    bool doWarn = true;
    if(calib.vemCal.empty() && useDefaults_)
    {
	log_warn("Using default VEM calibration values for all IceTop DOMs!");
	doWarn = false;
    }

    // Check vem calibration and fill up with default values
    std::map<OMKey, I3DOMCalibration>::const_iterator dom_iter;
    for(dom_iter=calib.domCal.begin(); dom_iter!=calib.domCal.end(); dom_iter++)
    {
	const OMKey& omKey = dom_iter->first;
	
	if(calib.vemCal.find(omKey)==calib.vemCal.end())
	{
	    if(useDefaults_)
	    {
		// If VEM calibration is missing set default values
		double pe_per_vem = 130.0;
		int staNum = omKey.GetString();
		if(staNum == 21 || staNum == 29 || staNum == 30 || staNum == 39)
		{
		    // For old stations
		    pe_per_vem = 220.0;
		}
		
		calib.vemCal[omKey].pePerVEM = pe_per_vem;
		calib.vemCal[omKey].muPeakWidth = 0.0;
		calib.vemCal[omKey].corrFactor = 1.0;
		calib.vemCal[omKey].hglgCrossOver = 3000.0;
		
		if(doWarn) log_warn("Missing VEM calibration values for %s. Using default values instead!", omKey.str().c_str());
	    }
	    else
	    {
		log_warn("Missing VEM calibration values for %s!", omKey.str().c_str());
	    }
	}
    }
}


void I3VEMCalManipulator::LoadDirectory(std::string path)
{
    vemCalList_.clear();
    
    // Remove unnecessary "/" characters from path
    while(path.find_last_of("/")==path.length()-1) path.erase(path.length()-1); 
    path += "/";
    
    DIR *dir = opendir(path.c_str());
    if(!dir)
    { 
	log_error("Unable to open directory \"%s\"!", path.c_str()); 
	return;
    }
    
    struct dirent *file;
    while((file = readdir(dir)))
    {
        std::string filename = path + file->d_name;
        
        if(filename.find(".xml")!=std::string::npos)
        {
            AddVEMCalibration(filename);
        }
    }
    closedir(dir);
    
    I3VEMCalList::iterator vemcal_iter;
    log_info("Loaded %zu VEM calibrations from \"%s\":", vemCalList_.size(), path.c_str());
    for(vemcal_iter = vemCalList_.begin(); vemcal_iter!=vemCalList_.end(); vemcal_iter++)
    {
	log_info("  %s", vemcal_iter->first.GetUTCString().c_str());
    }
}


void I3VEMCalManipulator::LoadFile(std::string filepath, bool forceIt)
{
    filepath_ = filepath;
    forceFile_ = forceIt;
    
    vemCalList_.clear();
    AddVEMCalibration(filepath_);
    
    if(vemCalList_.size()==1)
    {
	log_info("Loaded VEM calibration from %s", vemCalList_.begin()->first.GetUTCString().c_str());
    }
    else
    {
	vemCalList_.clear();
	log_error("Something went wrong while loading file \"%s\"", filepath_.c_str());
    }
}


bool I3VEMCalManipulator::AddVEMCalibration(const std::string& filename)
{
    // Open file	 
    std::ifstream ifs(filename.c_str(), std::ifstream::in);
    if(!ifs.good())
    {
	log_error("Could't open file \"%s\"!", filename.c_str());
	return false;
    }
    
    I3Time vemCalTime;
    I3VEMCalMap vemCal;
    
    // Find start
    int domCount=0;
    std::string line;
    do getline(ifs,line);
    while(line.find("<VEMCalibOm>")==std::string::npos && !ifs.eof());
    
    // Loop over entries
    while(line.find("</VEMCalibOm>")==std::string::npos && !ifs.eof())
    {
	getline(ifs,line);
	
	if(line.find("<Date>")!=std::string::npos)
	{
	    int year, month, day, hour, minute, second;
	    if(sscanf(line.c_str()," <Date> %d-%d-%d %d:%d:%d </Date>", &year, &month, &day, &hour, &minute, &second)==6)
	    {
		vemCalTime.SetUTCCalDate(year,month,day,hour,minute,second);
		log_debug("VEM Calibration date: %d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d", year, month, day, hour, minute, second); 
	    }
	}
	
	if(line.find("<DOM>")!=std::string::npos)
	{
	    domCount++;
	    
	    int str = -1;
	    int om  = -1;
	    double pePerVEM      = NAN;
	    double muWidth       = NAN;
	    double corrFactor    = NAN;
	    double hglgCrossOver = NAN;
	    
	    while(line.find("</DOM>")==std::string::npos && !ifs.eof())
	    {
		getline(ifs,line);
		
		if(line.find("<StringId>")!=std::string::npos)
		{
		    if(sscanf(line.c_str()," <StringId> %d </StringId>", &str)!=1) str = -1;
		}

		if(line.find("<TubeId>")!=std::string::npos)
		{
		    if(sscanf(line.c_str()," <TubeId> %d </TubeId>", &om)!=1) om = -1;
		}

		if(line.find("<pePerVEM>")!=std::string::npos)
		{
		    if(sscanf(line.c_str()," <pePerVEM> %lf </pePerVEM>", &pePerVEM)!=1) pePerVEM = NAN;
		}

		if(line.find("<muPeakWidth>")!=std::string::npos)
		{
		    if(sscanf(line.c_str()," <muPeakWidth> %lf </muPeakWidth>", &muWidth)!=1) muWidth = NAN;
		}
		
		if(line.find("<corrFactor>")!=std::string::npos)
		{
		    if(sscanf(line.c_str()," <corrFactor> %lf </corrFactor>", &corrFactor)!=1) corrFactor = NAN;
		}

		if(line.find("<hglgCrossOver>")!=std::string::npos)
		{
		    if(sscanf(line.c_str()," <hglgCrossOver> %lf </hglgCrossOver>", &hglgCrossOver)!=1) hglgCrossOver = NAN;
		}
	    }
	    
	    // Check and store the data
	    
	    if(!(str>0 && str<=80))
	    {
		log_error("Invalid string number (%d)!", str);
		continue;
	    }
	    
	    OMKey omKey(str,om);
	    
	    if(!finite(pePerVEM))
	    {
		log_error("Invalid VEM value for module %s!", omKey.str().c_str());
		continue;
	    }
	    
	    // Here only a warning since this value is usually not needed
	    if(!finite(muWidth))
	    {
		log_warn("Invalid muon peak width for module %s!", omKey.str().c_str());
	    }
	    
	    if(!finite(corrFactor))
	    {
		log_error("Invalid correction factor for module %s!", omKey.str().c_str());
		continue;
	    }

	    if(!finite(hglgCrossOver))
	    {
		log_error("Invalid HG-LG crossover value for module %s!", omKey.str().c_str());
		continue;
	    }
	    
	    // Check if DOM already exists in map
	    if(vemCal.find(omKey) != vemCal.end())
	    {
		log_error("Multiple VEM calibrations for module %s", omKey.str().c_str());
		continue;
	    }
	    
	    // Add data to map
	    vemCal[omKey].pePerVEM      = pePerVEM;
	    vemCal[omKey].muPeakWidth   = muWidth;
	    vemCal[omKey].corrFactor    = corrFactor;
	    vemCal[omKey].hglgCrossOver = hglgCrossOver;
	    
	    log_debug("VEM Calibration for %s: pePerVEM=%.2f pe, muPeakWidth=%.2f pe, corrFactor=%.2f, hglgCrossOver=%.2f pe",
		      omKey.str().c_str(), 
		      vemCal[omKey].pePerVEM,
		      vemCal[omKey].muPeakWidth,
		      vemCal[omKey].corrFactor,
		      vemCal[omKey].hglgCrossOver);
	}
    }
    ifs.close();
    
    if(vemCal.empty()) return false;
    
    if(vemCalTime==I3Time())
    {
	log_error("Couldn't determine VEM calibration time!");
	return false;
    }
    
    int checkDiff = domCount - vemCal.size();
    if(checkDiff > 0) log_warn("Missing VEM Calibration for %d DOMs!", checkDiff);
    
    // Add VEM calibration to list
    vemCalList_[vemCalTime] = vemCal;
    return true;
}
