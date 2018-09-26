#include <I3Test.h>

#include <string>
#include <sstream>

#include <dataclasses/I3Direction.h>
#include <dataclasses/I3MapOMKeyMask.h>
#include <dataclasses/I3MapOMKeyUnion.h>
#include <dataclasses/I3Matrix.h>
#include <dataclasses/I3Orientation.h>
#include <dataclasses/I3Position.h>
#include <dataclasses/I3Quaternion.h>
#include <dataclasses/I3Time.h>
#include <dataclasses/I3TimeWindow.h>
#include <dataclasses/I3RecoPulseSeriesMapApplySPECorrection.h>
#include <dataclasses/ModuleKey.h>
#include <dataclasses/TankKey.h>
#include <dataclasses/TriggerKey.h>
#include <dataclasses/geometry/I3OMGeo.h>
#include <dataclasses/geometry/I3ModuleGeo.h>
#include <dataclasses/geometry/I3TankGeo.h>
#include <dataclasses/physics/I3AMANDAAnalogReadout.h>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <dataclasses/physics/I3EventHeader.h>
#include <dataclasses/physics/I3FilterResult.h>
#include <dataclasses/physics/I3FlasherInfo.h>
#include <dataclasses/physics/I3MCHit.h>
#include <dataclasses/physics/I3MCTree.h>
#include <dataclasses/physics/I3Particle.h>
#include <dataclasses/physics/I3ParticleID.h>
#include <dataclasses/physics/I3RecoHit.h>
#include <dataclasses/physics/I3RecoPulse.h>
#include <dataclasses/physics/I3Trigger.h>
#include <dataclasses/physics/I3TWRFrag.h>
#include <dataclasses/physics/I3Waveform.h>

TEST_GROUP(PrintTest);

bool isGeneric(const std::string& s){
	return(s.find("I3FrameObject")!=std::string::npos);
}

template<typename Type, typename... Args>
void test_print(Args&&... args){
	Type obj(args...);
	std::ostringstream ss;
	obj.Print(ss);
	auto repr=ss.str();
	std::cout << "String representation:\n" << repr << std::endl;
	ENSURE(!repr.empty(),"Printing should not yield an empty string");
	ENSURE(!isGeneric(repr),"Class-specific printing should be implemented");
}

#define PrintTest(Type,...) \
TEST(print_ ## Type){ test_print<Type>(__VA_ARGS__); }

PrintTest(I3Direction,1,1,1);
PrintTest(I3RecoPulseSeriesMapMask);
PrintTest(I3RecoPulseSeriesMapUnion);
PrintTest(I3Matrix,4,4);
PrintTest(I3Orientation);
PrintTest(I3Position,1,2,3);
PrintTest(I3Quaternion,1,2,3,4);
PrintTest(I3OMGeo);
PrintTest(I3ModuleGeo);
PrintTest(I3TankGeo);
PrintTest(I3Time,2015,186358236758273652LL);
PrintTest(I3TimeWindow,67,99);
const static std::vector<I3TimeWindow> timeWindows={{1,2},{17,18},{31,39}};
PrintTest(I3TimeWindowSeries,timeWindows.begin(),timeWindows.end());
PrintTest(I3RecoPulseSeriesMapApplySPECorrection,"MyPulses","I3Calibration");
PrintTest(I3VectorBool,3,true);
PrintTest(I3VectorChar,5,'a');
PrintTest(I3VectorShort,10,5);
PrintTest(I3VectorUShort,10,5);
PrintTest(I3VectorInt,10,5);
PrintTest(I3VectorOMKey,2,OMKey{7,16});
PrintTest(ModuleKey,17,22);
PrintTest(TankKey,4,TankKey::TankB);
PrintTest(TriggerKey);
PrintTest(I3AMANDAAnalogReadout);
PrintTest(I3DOMLaunch);
PrintTest(I3EventHeader);
PrintTest(I3FilterResult);
PrintTest(I3FlasherInfo);
PrintTest(I3MCHit);
I3MCTree testMCTree;
static struct treeInitType{
  treeInitType(){
    I3Particle p1(I3Particle::Primary,I3Particle::Ti44Nucleus);
    I3Particle p2(I3Particle::StoppingTrack,I3Particle::NuMu);
    I3Particle p3(I3Particle::StoppingTrack,I3Particle::MuMinus);
    I3Particle p4(I3Particle::Cascade,I3Particle::Hadrons);
    testMCTree.insert(p1);
    testMCTree.append_child(p1,p2);
    testMCTree.append_child(p2,p3);
    testMCTree.append_child(p2,p4);
  }
} treeInit;
PrintTest(I3MCTree,testMCTree);
PrintTest(I3Particle,I3Particle::Primary,I3Particle::Ti44Nucleus);
PrintTest(I3ParticleID,124651251824ULL,62);
PrintTest(I3RecoHit);
PrintTest(I3RecoPulse);
PrintTest(I3Trigger);
PrintTest(I3TWRFrag);
PrintTest(I3Waveform);


