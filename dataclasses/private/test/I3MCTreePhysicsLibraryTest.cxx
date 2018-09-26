/**
 * @file I3MCTreeTest.cxx
 * @brief
 * 
 * copyright (C) 2013 the icecube collaboration
 * 
 * $Id: I3MCTreeTest.cxx 127835 2015-01-14 23:38:24Z david.schultz $
 * @version $Revision: 127835 $
 * @date $Date: 2015-01-14 16:38:24 -0700 (Wed, 14 Jan 2015) $
 */
#include <I3Test.h>
#include <dataclasses/physics/I3MCTree.h>
#include <dataclasses/physics/I3MCTreeUtils.h>
#include <dataclasses/physics/I3MCTreePhysicsLibrary.hh>

using namespace std;

TEST_GROUP(I3MCTreePhysicsLibraryTest);

I3MCTree make_tree(){

  I3MCTree t;

  I3Particle fe56_nucleus;
  fe56_nucleus.SetType(I3Particle::Fe56Nucleus);
  fe56_nucleus.SetShape(I3Particle::Primary);
  fe56_nucleus.SetEnergy(1.);
  t.insert(fe56_nucleus);

  I3Particle he3_nucleus;
  he3_nucleus.SetType(I3Particle::He3Nucleus);
  he3_nucleus.SetShape(I3Particle::Primary);
  he3_nucleus.SetEnergy(2.);
  t.insert(he3_nucleus);

  I3Particle proton;
  proton.SetType(I3Particle::PPlus);
  proton.SetShape(I3Particle::Primary);
  proton.SetEnergy(3.);
  t.insert(proton);

  I3Particle nu_mu;
  nu_mu.SetType(I3Particle::NuMu);
  nu_mu.SetShape(I3Particle::Primary);
  nu_mu.SetEnergy(4.);
  t.insert(nu_mu);

  I3Particle nu_e;
  nu_e.SetType(I3Particle::NuE);
  nu_e.SetShape(I3Particle::Primary);
  nu_e.SetEnergy(5.);
  t.insert(nu_e);

  I3Particle in_ice_muon;
  in_ice_muon.SetType(I3Particle::MuMinus);
  in_ice_muon.SetLocationType(I3Particle::InIce);
  in_ice_muon.SetEnergy(6.);
  t.append_child(nu_mu, in_ice_muon);

  I3Particle mu_plus;
  mu_plus.SetType(I3Particle::MuPlus);
  mu_plus.SetEnergy(7.);
  t.append_child(nu_mu, mu_plus);

  I3Particle in_ice_hadron;
  in_ice_hadron.SetType(I3Particle::Hadrons);
  in_ice_hadron.SetLocationType(I3Particle::InIce);
  in_ice_hadron.SetEnergy(8.);
  t.append_child(nu_mu, in_ice_hadron);

  I3Particle low_e_electron;
  low_e_electron.SetType(I3Particle::EMinus);
  low_e_electron.SetEnergy(9.);
  t.append_child(nu_mu, low_e_electron);

  I3Particle high_e_electron;
  high_e_electron.SetType(I3Particle::EMinus);
  high_e_electron.SetEnergy(10.);
  t.append_child(nu_mu, high_e_electron);

  return t;  
}

using I3MCTreePhysicsLibrary::GetMostEnergeticPrimary;
using I3MCTreePhysicsLibrary::GetMostEnergeticInIce;
using I3MCTreePhysicsLibrary::GetMostEnergetic;
using I3MCTreePhysicsLibrary::GetMostEnergeticTrack;
using I3MCTreePhysicsLibrary::GetMostEnergeticCascade;
using I3MCTreePhysicsLibrary::GetMostEnergeticInIceCascade;
using I3MCTreePhysicsLibrary::GetMostEnergeticNeutrino;
using I3MCTreePhysicsLibrary::GetMostEnergeticMuon;
using I3MCTreePhysicsLibrary::GetMostEnergeticNucleus;

template <class T>
void
test_get_most_energetic(T t){
  boost::optional<I3Particle> me_primary = GetMostEnergeticPrimary(t);
  ENSURE( (bool)me_primary, "GetMostEnergeticPrimary did not return a particle.");
  ENSURE( me_primary->GetType() == I3Particle::NuE , "this is not a nu_e");

  boost::optional<I3Particle> me_inice = GetMostEnergeticInIce(t);
  ENSURE( (bool)me_inice, "GetMostEnergeticInIce did not return a particle.");
  ENSURE( me_inice->GetType() == I3Particle::Hadrons , "this is not a hadron");

  boost::optional<I3Particle> me_e = GetMostEnergetic(t, I3Particle::EMinus);
  ENSURE( (bool)me_e, "GetMostEnergetic did not return a particle.");
  ENSURE( me_e->GetEnergy() > 9. , "this is not the right electron");

  boost::optional<I3Particle> me_track = GetMostEnergeticTrack(t);
  ENSURE( (bool)me_track, "GetMostEnergetic did not return a particle.");
  ENSURE( me_track->GetType() == I3Particle::MuPlus , "this is not the mu+");

  boost::optional<I3Particle> me_cascade = GetMostEnergeticCascade(t);
  ENSURE( (bool)me_cascade, "GetMostEnergeticCascade did not return a particle.");
  ENSURE( me_cascade->GetType() == I3Particle::EMinus , "this is not the electron");

  boost::optional<I3Particle> me_inice_cascade = GetMostEnergeticInIceCascade(t);
  ENSURE( (bool)me_inice_cascade, "GetMostEnergeticInIceCascade did not return a particle.");
  ENSURE( me_inice_cascade->GetType() == I3Particle::Hadrons , "this is not the hadron");

  boost::optional<I3Particle> me_neutrino = GetMostEnergeticNeutrino(t);
  ENSURE( (bool)me_neutrino, "GetMostEnergeticNeutrino did not return a particle.");
  ENSURE( me_neutrino->GetType() == I3Particle::NuE , "this is not the nu_e");

  boost::optional<I3Particle> me_muon = GetMostEnergeticMuon(t);
  ENSURE( (bool)me_muon, "GetMostEnergeticMuon did not return a particle.");
  ENSURE( me_muon->GetType() == I3Particle::MuPlus , "this is not the mu+");

  boost::optional<I3Particle> me_nucleus = GetMostEnergeticNucleus(t);
  ENSURE( (bool)me_nucleus, "GetMostEnergeticNucleus did not return a particle.");
  ENSURE( me_nucleus->GetType() == I3Particle::He3Nucleus , "this is not the He3");

}

TEST(get_most_energetic){
  I3MCTree t(make_tree());
  test_get_most_energetic<I3MCTree>(t);
}

TEST(get_most_energetic_from_tree_ptr){
  I3MCTreePtr t(new I3MCTree(make_tree()));
  test_get_most_energetic<I3MCTreePtr>(t);
}

TEST(multiple_most_energetic){
  I3MCTree t(make_tree());
  boost::optional<I3Particle> mep = GetMostEnergeticPrimary(t);
  ENSURE(mep != boost::none);

  I3Particle other_mep = mep->Clone();
  t.insert(other_mep);

  ENSURE(GetMostEnergeticPrimary(t) == boost::none);
}
