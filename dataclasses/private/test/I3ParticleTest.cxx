/**
    copyright  (C) 2004
    the icecube collaboration

    @version $Revision$
    @date $Date$
    @author blaufuss

    @todo
*/

#include <I3Test.h>
#include <iostream>
#include <string>
#include <boost/preprocessor.hpp>

#include "dataclasses/physics/I3Particle.h"
#include "dataclasses/I3Constants.h"
#include "icetray/I3Units.h"

using namespace std;

class test_particle_id_private_ctor{
public:
  I3Particle particle;
  test_particle_id_private_ctor(const uint64_t major, const int32_t minor) :
    particle(major, minor){};
};

TEST_GROUP(I3Particle);

TEST(CopyPtrConstructor)
{

  I3ParticlePtr p1(new I3Particle());
  p1->SetPos(1.0, 2.0, 3.0);
  p1->SetDir(2.5,3.5);
  p1->SetTime(0.0);
  p1->SetLength(10.0);
  p1->SetType(I3Particle::PMinus);
  p1->SetShape(I3Particle::Cascade);
  p1->SetEnergy(100*I3Units::GeV);
  p1->SetSpeed(I3Constants::c);
  cout<<AsXML(p1);

  I3ParticlePtr p2(p1);
  cout<<AsXML(p2);

  ENSURE_DISTANCE(p1->GetX(),p2->GetX(),0.01,
                  "Copied position (X) do not match");
  ENSURE_DISTANCE(p1->GetY(),p2->GetY(),0.01,
                  "Copied position (Y) do not match");
  ENSURE_DISTANCE(p1->GetZ(),p2->GetZ(),0.01,
                  "Copied position (Z) do not match");
  ENSURE_DISTANCE(p1->GetZenith(),p2->GetZenith(),0.01,
                  "Copied direction (Zenith) does not match");
  ENSURE_DISTANCE(p1->GetAzimuth(),p2->GetAzimuth(),0.01,
                  "Copied direction (Azimuth) does not match");
  ENSURE_DISTANCE(p1->GetEnergy(),p2->GetEnergy(),0.01,
                  "Copied energy does not match");
  ENSURE_DISTANCE(p1->GetSpeed(),p2->GetSpeed(),0.01,
                  "Copied speed does not match");
  ENSURE( p1->GetType() == p2->GetType(),
                  "Copied type do not match");
  ENSURE( p1->GetShape() == p2->GetShape(),
                  "Copied shape do not match");
  ENSURE( p1 == p2, 
	  "Copied particles overall do not match");

}

TEST(CopyObjConstructor)
{

  I3Particle p1;
  p1.SetPos(1.0, 2.0, 3.0);
  p1.SetDir(2.5,3.5);
  p1.SetTime(0.0);
  p1.SetLength(10.0);
  p1.SetType(I3Particle::PMinus);
  p1.SetShape(I3Particle::Cascade);
  p1.SetEnergy(100*I3Units::GeV);
  p1.SetSpeed(I3Constants::c);
  cout<<AsXML(p1);

  I3Particle p2(p1);
  cout<<AsXML(p2);

  ENSURE_DISTANCE(p1.GetX(),p2.GetX(),0.01,
                  "Copied position (X) do not match");
  ENSURE_DISTANCE(p1.GetY(),p2.GetY(),0.01,
                  "Copied position (Y) do not match");
  ENSURE_DISTANCE(p1.GetZ(),p2.GetZ(),0.01,
                  "Copied position (Z) do not match");
  ENSURE_DISTANCE(p1.GetZenith(),p2.GetZenith(),0.01,
                  "Copied direction (Zenith) does not match");
  ENSURE_DISTANCE(p1.GetAzimuth(),p2.GetAzimuth(),0.01,
                  "Copied direction (Azimuth) does not match");
  ENSURE_DISTANCE(p1.GetEnergy(),p2.GetEnergy(),0.01,
                  "Copied energy does not match");
  ENSURE_DISTANCE(p1.GetSpeed(),p2.GetSpeed(),0.01,
                  "Copied speed does not match");
  ENSURE( p1.GetType() == p2.GetType(),
                  "Copied type do not match");
  ENSURE( p1.GetShape() == p2.GetShape(),
                  "Copied shape do not match");
  ENSURE( p1.GetID() == p2.GetID(),
                  "Particle IDs do not match");
  ENSURE( p1 == p2,
	          "Particles overall do not match");

}

TEST(pid_ctor){
  uint64_t major = 6636784257750396848;
  int32_t minor = 42;
  test_particle_id_private_ctor test_particle(major, minor);
  ENSURE(test_particle.particle.GetMajorID() == major);
  ENSURE(test_particle.particle.GetMinorID() == minor);
}

TEST(istrack){

  I3Particle itrack; 
  I3Particle start_track;
  I3Particle stop_track;
  I3Particle cont_track;
  I3Particle muplus;
  I3Particle muminus;
  I3Particle tauplus;
  I3Particle tauminus;
  I3Particle mp;

  itrack.SetShape(I3Particle::InfiniteTrack);
  start_track.SetShape(I3Particle::StartingTrack);
  stop_track.SetShape(I3Particle::StoppingTrack);
  cont_track.SetShape(I3Particle::ContainedTrack);
  muplus.SetType(I3Particle::MuPlus);
  muminus.SetType(I3Particle::MuMinus);
  tauplus.SetType(I3Particle::TauPlus);
  tauminus.SetType(I3Particle::MuMinus);
  mp.SetType(I3Particle::Monopole);

  ENSURE(itrack.IsTrack()); 
  ENSURE(start_track.IsTrack());
  ENSURE(stop_track.IsTrack());
  ENSURE(cont_track.IsTrack());
  ENSURE(muplus.IsTrack());
  ENSURE(muminus.IsTrack());
  ENSURE(tauplus.IsTrack());
  ENSURE(tauminus.IsTrack());
  ENSURE(mp.IsTrack());
  

}

TEST(iscascade){

  I3Particle cascade; 
  I3Particle eplus; 
  I3Particle eminus; 
  I3Particle brem; 
  I3Particle delta; 
  I3Particle pair; 
  I3Particle nucl; 
  I3Particle gamma; 
  I3Particle hadron; 
  I3Particle piplus; 
  I3Particle piminus; 

  cascade.SetShape(I3Particle::Cascade);
  eplus.SetType(I3Particle::EPlus); 
  eminus.SetType(I3Particle::EMinus); 
  brem.SetType(I3Particle::Brems); 
  delta.SetType(I3Particle::DeltaE); 
  pair.SetType(I3Particle::PairProd); 
  nucl.SetType(I3Particle::NuclInt); 
  gamma.SetType(I3Particle::Gamma); 
  hadron.SetType(I3Particle::Hadrons); 
  piplus.SetType(I3Particle::PiPlus); 
  piminus.SetType(I3Particle::PiMinus); 

  ENSURE(cascade.IsCascade()); 
  ENSURE(eplus.IsCascade()); 
  ENSURE(eminus.IsCascade()); 
  ENSURE(brem.IsCascade()); 
  ENSURE(delta.IsCascade()); 
  ENSURE(pair.IsCascade()); 
  ENSURE(nucl.IsCascade()); 
  ENSURE(gamma.IsCascade()); 
  ENSURE(hadron.IsCascade()); 
  ENSURE(piplus.IsCascade()); 
  ENSURE(piminus.IsCascade()); 
}

TEST(shifting_tracks) {
  I3Particle p(I3Position(0.,0.,0.), I3Direction(1.,1.,1.), 0., I3Particle::ContainedTrack, 20.);
  ENSURE(p.ShiftTimeTrack(0)== I3Position(0.,0.,0.) );
  // 1/sqrt(3)*10*I3Constants::c == 1.73085
  ENSURE( (p.ShiftTimeTrack(10)-I3Position(1.73085,1.73085,1.73085)).Magnitude() <0.001 ); //precision better than a mm
}


TEST(get_mass_for_type){
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Gamma), 0., 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::EPlus), 0.00051099891*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::EMinus), 0.00051099891*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::MuPlus), 0.105658367*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::MuMinus), 0.105658367*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Pi0), 0.1349766*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::PiPlus), 0.1395701*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::PiMinus), 0.1395701*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::K0_Long), 0.497614*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::KPlus), 0.493677*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::KMinus), 0.493677*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Neutron), 0.93956536*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::PPlus), 0.938272013*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::PMinus), 0.938272013*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::K0_Short), 0.497614*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Eta), 0.547853*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Lambda), 1.115683*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::SigmaPlus), 1.18937*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Sigma0), 1.192642*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::SigmaMinus), 1.18937*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Xi0), 1.31486*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::XiMinus), 1.32171*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::OmegaMinus), 1.67245*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::NeutronBar), 0.93956536*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::LambdaBar), 1.115683*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::SigmaMinusBar), 1.18937*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Sigma0Bar), 1.192642*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::SigmaPlusBar), 1.18937*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Xi0Bar), 1.31486*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::XiPlusBar), 1.32171*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::OmegaPlusBar), 1.67245*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::DPlus), 1.86957*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::DMinus), 1.86957*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::D0), 1.8648*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::D0Bar), 1.8648*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::DsPlus), 1.96845*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::DsMinusBar), 1.96845*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::LambdacPlus), 2.28646*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::WPlus), 80.385*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::WMinus), 80.385*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Z0), 91.1876*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::NuE), 0.0*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::NuEBar), 0.0*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::NuMu), 0.0*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::NuMuBar), 0.0*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::TauPlus), 1.77682*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::TauMinus), 1.77682*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::NuTau), 0.0*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::NuTauBar), 0.0*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::H2Nucleus),  1.875613*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::He3Nucleus), 2.808391*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::He4Nucleus), 3.727379*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Li6Nucleus), 5.60151816372*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Li7Nucleus), 6.53383353972*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Be9Nucleus), 8.39275030104*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::B10Nucleus), 9.32443669262*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::B11Nucleus), 10.2525479206*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::C12Nucleus), 11.174863388*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::C13Nucleus), 12.1094824273*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::N14Nucleus), 13.0402043278*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::N15Nucleus), 13.9689363768*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::O16Nucleus), 14.8950815346*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::O17Nucleus), 15.830503751*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::O18Nucleus), 16.76202507*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::F19Nucleus), 17.6923029112*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ne20Nucleus), 18.6177321841*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ne21Nucleus), 19.5505363674*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ne22Nucleus), 20.4797374564*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Na23Nucleus), 21.4092162538*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Mg24Nucleus), 22.3357965987*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Mg25Nucleus), 23.2680313677*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Mg26Nucleus), 24.1965036397*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Al26Nucleus), 24.1999980695*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Al27Nucleus), 25.1265057485*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Si28Nucleus), 26.0531939251*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Si29Nucleus), 26.9842857039*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Si30Nucleus), 27.9132418499*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Si31Nucleus), 28.8462197999*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Si32Nucleus), 29.7765819269*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::P31Nucleus), 28.8442183429*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::P32Nucleus), 29.7758480379*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::P33Nucleus), 30.7053097979*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::S32Nucleus), 29.773628119*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::S33Nucleus), 30.70455185*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::S34Nucleus), 31.632700084*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::S35Nucleus), 32.565279544*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::S36Nucleus), 33.494955853*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Cl35Nucleus), 32.5646030619*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Cl36Nucleus), 33.4955887729*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Cl37Nucleus), 34.4248431259*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ar36Nucleus), 33.4943699372*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ar37Nucleus), 34.4251478462*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ar38Nucleus), 35.3528749822*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ar39Nucleus), 36.2858415502*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ar40Nucleus), 37.2155376931*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ar41Nucleus), 38.1490041502*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ar42Nucleus), 39.0791429702*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::K39Nucleus), 36.284767546*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::K40Nucleus), 37.21653338*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::K41Nucleus), 38.146003539*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ca40Nucleus), 37.2147134578*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ca41Nucleus), 38.1459160018*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ca42Nucleus), 39.0740007168*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ca43Nucleus), 40.0056331778*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ca44Nucleus), 40.9340673658*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ca45Nucleus), 41.8662179228*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ca46Nucleus), 42.7953888238*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ca47Nucleus), 43.7276778058*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ca48Nucleus), 44.6572978278*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Sc44Nucleus), 40.9372110547*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Sc45Nucleus), 41.8654533837*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Sc46Nucleus), 42.7962580887*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Sc47Nucleus), 43.7251771107*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Sc48Nucleus), 44.6565071587*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ti44Nucleus), 40.9369701498*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ti45Nucleus), 41.8670068998*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ti46Nucleus), 42.7933832428*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ti47Nucleus), 43.7240682988*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ti48Nucleus), 44.6520069938*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ti49Nucleus), 45.5834299498*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Ti50Nucleus), 46.5120561048*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::V48Nucleus), 44.6555109582*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::V49Nucleus), 45.5835234282*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::V50Nucleus), 46.5137528452*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::V51Nucleus), 47.4422670442*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Cr50Nucleus), 46.5122066868*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Cr51Nucleus), 47.4425114068*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Cr52Nucleus), 48.3700373088*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Cr53Nucleus), 49.3016635288*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Cr54Nucleus), 50.2315097528*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Mn52Nucleus), 48.3742407516*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Mn53Nucleus), 49.3017523196*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Mn54Nucleus), 50.2323788816*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Mn55Nucleus), 51.1617176996*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Fe54Nucleus), 50.2311739195*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Fe55Nucleus), 51.1614410355*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Fe56Nucleus), 52.0898090795*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Fe57Nucleus), 53.0217283295*I3Units::GeV, 1*I3Units::eV, "The mass changed!");
  ENSURE_DISTANCE(I3Particle::GetMassForType(I3Particle::Fe58Nucleus), 53.9512490695*I3Units::GeV, 1*I3Units::eV, "The mass changed!");;
}


TEST( settypestring )
{
    I3Particle numu;

    numu.SetTypeString("NuMu");
    
    ENSURE( numu.GetTypeString() == "NuMu" );
}


TEST(hasenergy){

  I3Particle particle;
 
  particle.SetEnergy(105.0*I3Units::MeV);

  ENSURE(particle.HasEnergy());
}

TEST(stop_time) {
  I3Particle particle;
  ENSURE(std::isnan(particle.GetStopTime()));

  particle.SetShape(I3Particle::StoppingTrack);
  double time = 10.*I3Units::ns;
  particle.SetTime(time);
  ENSURE_EQUAL(particle.GetStopTime(), time);

  particle.SetShape(I3Particle::ContainedTrack);
  double length = 500.*I3Units::m;
  particle.SetLength(length);
  time += length/I3Constants::c;
  ENSURE_EQUAL(particle.GetStopTime(), time);

  particle.SetShape(I3Particle::MCTrack);
  ENSURE_EQUAL(particle.GetStopTime(), time);
}

TEST(istopshower){

  I3Particle istopshower; 
  istopshower.SetShape(I3Particle::TopShower);
  ENSURE(istopshower.IsTopShower()); 

}

