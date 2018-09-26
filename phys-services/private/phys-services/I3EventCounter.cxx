#include <sstream>
#include <fstream>
#include "phys-services/I3EventCounter.h"
#include "dataclasses/physics/I3EventHeader.h"

using std::ostringstream;

I3_MODULE(I3EventCounter);


I3EventCounter :: I3EventCounter(const I3Context& ctx) : 
		I3Module(ctx),
		physCount_(0),
		statusCount_(0),
		geometryCount_(0),
		calibCount_(0),
		counterStep_(100),
		nevents_(0),
		dump_(false),
		eventHeaderName_("I3EventHeader"),
		physicsCounterName_("Events"),
		geoCounterName_(""),
		calibCounterName_(""),
		statusCounterName_("")
{
  AddOutBox("OutBox");

  path_="stdout";
  AddParameter("filename","The file we'll write to or ('stdout' | 'stderr').  ", path_);

  fmt_ ="\nphysics frames: %d"; 
  fmt_+="\ngeometry frames: %d"; 
  fmt_+="\ncalibration frames: %d";
  fmt_+="\ndetector status frames: %d";
  AddParameter("formatstr","Format string for frame counts.  ", fmt_);

  AddParameter("CounterStep",
               "Only print out event number if divisible by this",
	       counterStep_);

  AddParameter("Dump",
               "Whether to dump the current frame to screen",
               dump_);

  AddParameter("NEvents",
               "Number of events to process",
               nevents_);

  AddParameter("EventHeaderName",
               "Name of event header in the frame",
               eventHeaderName_);
  AddParameter("PhysicsCounterName",
               "String label for physics frame counter",
               physicsCounterName_);
  AddParameter("GeometryCounterName",
               "String label for geometry frame counter",
               geoCounterName_);
  AddParameter("CalibrationCounterName",
               "String label for calibration frame counter",
               calibCounterName_);
  AddParameter("DetectorStatusCounterName",
               "String label for detector status frame counter",
               statusCounterName_);
}


void I3EventCounter :: Configure()
{
  GetParameter("filename", path_);
  log_info("(%s) Writting event count to : %s",
           GetName().c_str(), path_.c_str());

  GetParameter("formatstr", fmt_);
  log_info("(%s) : How counter output should be formatted. %s",
           GetName().c_str(), fmt_.c_str());

  GetParameter("CounterStep", counterStep_);
  log_info("(%s) Event Counter Step: %i",
           GetName().c_str(), counterStep_);

  GetParameter("Dump", dump_);
  log_info("(%s) Whether to dump frame to screen: %i",
           GetName().c_str(), dump_);

  GetParameter("NEvents", nevents_);
  log_info("(%s) NEvents: %i",
           GetName().c_str(), nevents_);

  GetParameter("EventHeaderName",
               eventHeaderName_);
  GetParameter("PhysicsCounterName",
               physicsCounterName_);
  GetParameter("GeometryCounterName",
               geoCounterName_);
  GetParameter("CalibrationCounterName",
               calibCounterName_);
  GetParameter("DetectorStatusCounterName",
               statusCounterName_);

  if (path_ == "stdout") {
	  out = &std::cout;
  }
  else if (path_ == "stderr") {
	  out = &std::cerr;
  }
  else {
	  out = new std::ofstream(path_.c_str(), std::ios::out);
  }
}

namespace {
  const char* myordinal(int i){
    static char ordinal[256];
    if ( ( (i%10) > 3 ) || ((i%10)==0) || (((i/10)%10)==1) ){
      sprintf(ordinal,"%dth",i);
    } else {
      switch(i%10){
      case 1:
	sprintf(ordinal,"%dst",i); break;
      case 2:
	sprintf(ordinal,"%dnd",i); break;
      case 3:
	sprintf(ordinal,"%drd",i); break;
      default:
	log_fatal("programming error i=%d",i);
      }
    }
    return ordinal;
  }
}
void I3EventCounter :: Physics(I3FramePtr frame)
{
  physCount_++;

  I3EventHeaderConstPtr eh = frame->Get<I3EventHeaderConstPtr>(eventHeaderName_);
  if (eh) {  // Frame might not have an event header
	  if (physCount_%counterStep_ == 0) {
	    log_info("(%s) Processing %s event (EventID=%i, RunID=%i)",
		     GetName().c_str(),myordinal(physCount_),
		     eh->GetEventID(),
		     eh->GetRunID());
	  }
  }

  if (dump_) {
    ostringstream oss;
    oss << *frame;
    log_info("(%s) Current frame (%d):\n%s",
             GetName().c_str(), physCount_, oss.str().c_str() );
  }

  if (physCount_ >= nevents_ && nevents_ != 0) RequestSuspension();

  PushFrame(frame,"OutBox");
}

void I3EventCounter::Geometry(I3FramePtr frame)
{
  geometryCount_++;  
  log_debug("frame %d", geometryCount_);  

  PushFrame(frame,"OutBox");
}

void I3EventCounter::Calibration(I3FramePtr frame)
{
  calibCount_++;  
  log_debug("frame %d", calibCount_);  

  PushFrame(frame,"OutBox");
}

void I3EventCounter::DetectorStatus(I3FramePtr frame)
{
  statusCount_++;  
  log_debug("frame %d", statusCount_);  

  PushFrame(frame,"OutBox");
}


void I3EventCounter :: Finish()
{
  log_info("(%s) Total number of physics events:  %d",
           GetName().c_str(), physCount_);

  // format string with values
  sprintf(buffer,fmt_.c_str(),physCount_,geometryCount_,calibCount_,statusCount_);

  if (!summary_) summary_= context_.Get<I3MapStringDoublePtr>("I3SummaryService");

  // Write values to summary service
  if (summary_) {
	  if (!physicsCounterName_.empty()) (*summary_)[physicsCounterName_] += physCount_;
	  if (!geoCounterName_.empty()) (*summary_)[geoCounterName_] += geometryCount_;
	  if (!calibCounterName_.empty()) (*summary_)[calibCounterName_] += calibCount_;
	  if (!statusCounterName_.empty()) (*summary_)[statusCounterName_] += statusCount_;
  } 
  else { // No summary service found
	  log_warn("No I3SummaryService found.");
	  // out put string to stream
	  (*out) << buffer << std::endl;
  }


}
