/**
 * Copyright (C) 2008
 * The IceCube collaboration
 * ID: $Id$
 *
 * @file I3VEMCalManipulator.h
 * @version $Revision$
 * @date $Date$
 * @author $Author$
 */


#ifndef PHYS_SERVICES_I3VEMCALMANIPULATOR_H_INCLUDED
#define PHYS_SERVICES_I3VEMCALMANIPULATOR_H_INCLUDED

#include <interfaces/I3CalibrationService.h>
#include <dataclasses/calibration/I3Calibration.h>
#include <map>

/**
 *  @brief This calibration service copies all the entries from a previous  
 *  calibration service and replaces the VEM calibration by values from an 
 *  XML file or by default values. XML files containing VEM calibration data can be found in the  
 *  <a href="http://convey.icecube.wisc.edu/data/exp/IceCube/2010/calibration/VEMCal/">data
 *  warehouse</a>.
 */

class I3VEMCalManipulator: public I3CalibrationService
{
public:
    I3VEMCalManipulator(I3CalibrationService& calService);
    ~I3VEMCalManipulator();
    
    void LoadDirectory(std::string path);
    void LoadFile(std::string filepath, bool forceIt=false);
    void UseDefaults(bool useDefault) {useDefaults_=useDefault;};
    
    I3CalibrationConstPtr GetCalibration(I3Time time);
    
private:
    
    bool AddVEMCalibration(const std::string& filename);
    void SetVEMCalibration(I3Calibration& calib, const I3Time& time);
    void CheckVEMCalibration(I3Calibration& calib);
    
    I3CalibrationService& calService_;
    I3CalibrationPtr currentCalib_;
    
    std::string filepath_;
    bool forceFile_;
    bool useDefaults_;
    
    typedef std::map<OMKey, I3VEMCalibration> I3VEMCalMap;
    typedef std::map<I3Time, I3VEMCalMap> I3VEMCalList;
    I3VEMCalList vemCalList_;
    
    SET_LOGGER("I3VEMCalManipulator");
    
};

I3_DEFAULT_NAME(I3VEMCalManipulator)

#endif
