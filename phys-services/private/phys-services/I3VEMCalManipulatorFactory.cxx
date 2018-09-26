#include <phys-services/I3VEMCalManipulatorFactory.h>

I3_SERVICE_FACTORY(I3VEMCalManipulatorFactory);

I3VEMCalManipulatorFactory::I3VEMCalManipulatorFactory(const I3Context& context): I3ServiceFactory(context)
{
    vemCalService_ = boost::shared_ptr<I3VEMCalManipulator>();
    
    inCalServiceName_ = I3DefaultName<I3CalibrationService>::value();
    AddParameter("InputCalibrationService","Name of the input calibration service.", inCalServiceName_);	
    
    outCalServiceName_ = "CalibrationWithVEMCal";
    AddParameter("OutputCalibrationService","Name of the output calibration service.", outCalServiceName_);
    
    path_ = "";
    AddParameter("Directory","Location of the VEMCal XML-files.", path_);
    
    file_ = "";
    AddParameter("File","Path to an individual VEMCal file.", file_);
    
    forceFile_ = "";
    AddParameter("ForceFile","Enforce usage of file disregarding the calibration validity period.", forceFile_);

    useDefaults_ = false;
    AddParameter("UseDefaults","Replace missing calibrations by default values.", useDefaults_);
    
}

I3VEMCalManipulatorFactory:: ~I3VEMCalManipulatorFactory()
{}

void I3VEMCalManipulatorFactory::Configure()
{
    GetParameter("InputCalibrationService", inCalServiceName_);
    GetParameter("OutputCalibrationService", outCalServiceName_);	
    GetParameter("Directory", path_);
    GetParameter("File", file_);
    GetParameter("ForceFile", forceFile_);
    GetParameter("UseDefaults", useDefaults_);
    
    if(!file_.empty() && !path_.empty())
    {
	log_fatal("You can either select an individual VEMCal file or a directory containing multiple files. "
		  "Please check your phthon script!");
    }
}

bool I3VEMCalManipulatorFactory::InstallService(I3Context& context)
{
    if(!vemCalService_)
    {
	I3CalibrationService& calService = context_.Get<I3CalibrationService>(inCalServiceName_);
	vemCalService_ = boost::shared_ptr<I3VEMCalManipulator>(new I3VEMCalManipulator(calService));
	if(!path_.empty()) vemCalService_->LoadDirectory(path_);
	if(!file_.empty()) vemCalService_->LoadFile(file_, forceFile_);
	vemCalService_->UseDefaults(useDefaults_);
    }
    
    return context.Put<I3CalibrationService>(vemCalService_, outCalServiceName_);
}
