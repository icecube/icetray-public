#include <dataclasses/ttree_oarchive.h>

//#include <icetray/test/random_iarchive.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/is_abstract.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include <boost/random/lagged_fibonacci.hpp>
#include <boost/random/mersenne_twister.hpp>

#include <dataclasses/BoostHeaders.h>

// dc headers must come last
#include <dataclasses/I3DoubleBang.h>

static boost::lagged_fibonacci607 random_double;
static boost::mt19937             random_int;

static void reseed()
{
  random_double.seed();
  random_int.seed();
}

#include <I3Test.h>
#include <fstream>

using boost::serialization::make_nvp;
using boost::serialization::base_object;

#include <TFile.h>
#include <TTree.h>
#include <boost/preprocessor.hpp>

TEST_GROUP(ttree_doublebang);

const string fname = "I3DoubleBang.root";
const unsigned nentries = 50000;

static void randomize_db(I3DoubleBang& db)
{
  db.SetStartPos(random_double(), random_double(), random_double());
  db.SetEnergy(random_double());
  db.SetLength(random_double());
}

TEST(doublebang_shared_ptr_0)
{
  reseed();
  unlink (fname.c_str());
  TFile tfile(fname.c_str(), "RECREATE");
  TTree ttree("doublebang", "", 0);
  ttree_oarchive oa(ttree);
  
  I3DoubleBangPtr bangs[10];
  for (unsigned i=0; i<10; i++)
    bangs[i] = I3DoubleBangPtr(new I3DoubleBang);

  for (unsigned i=0; i<nentries; i++)
    {
      randomize_db(*(bangs[i%10]));
      I3ParticlePtr particle(bangs[i%10]);
      oa << make_nvp("doublebang", particle);
    }
  ttree.Write();
  tfile.Close();
}

TEST(doublebang_shared_ptr_1)
{
  reseed();
  TFile tfile(fname.c_str());
  TTree* tree = (TTree*)tfile.Get("doublebang");
  
  double x, y, z, e, len;

  tree->SetBranchAddress("X", &x);
  tree->SetBranchAddress("Y", &y);
  tree->SetBranchAddress("Z", &z);
  tree->SetBranchAddress("Energy", &e);
  tree->SetBranchAddress("Length", &len);

  ENSURE_EQUAL(tree->GetEntries(), nentries);

  for (unsigned j=0; j<nentries; j++)
    {
      I3DoubleBang db;
      randomize_db(db);
      tree->GetEntry(j);
      ENSURE_DISTANCE(e, db.GetEnergy(), 0.00001);
      ENSURE_DISTANCE(x, db.GetStartPos().GetX(), 0.00001);
      ENSURE_DISTANCE(y, db.GetStartPos().GetY(), 0.00001);
      ENSURE_DISTANCE(z, db.GetStartPos().GetZ(), 0.00001);
      ENSURE_DISTANCE(len, db.GetLength(), 0.00001);
    }
}

TEST(doublebang_bald_ptr_0)
{
  reseed();
  unlink (fname.c_str());
  TFile tfile(fname.c_str(), "RECREATE");
  TTree ttree("doublebang", "", 0);
  ttree_oarchive oa(ttree);
  
  I3DoubleBang* bangs[10];
  for (unsigned i=0; i<10; i++)
    bangs[i] = new I3DoubleBang;

  for (unsigned i=0; i<nentries; i++)
    {
      randomize_db(*(bangs[i%10]));
      I3Particle* particle = bangs[i%10];
      oa << make_nvp("doublebang", particle);
    }
  ttree.Write();
  tfile.Close();

  for (int i=0; i<10; i++)
    delete bangs[i];
}

TEST(doublebang_bald_ptr_1)
{
  reseed();
  TFile tfile(fname.c_str());
  TTree* tree = (TTree*)tfile.Get("doublebang");
  
  double x, y, z, e, len;

  tree->SetBranchAddress("X", &x);
  tree->SetBranchAddress("Y", &y);
  tree->SetBranchAddress("Z", &z);
  tree->SetBranchAddress("Energy", &e);
  tree->SetBranchAddress("Length", &len);

  ENSURE_EQUAL(tree->GetEntries(), nentries);

  for (unsigned j=0; j<nentries; j++)
    {
      I3DoubleBang db;
      randomize_db(db);
      tree->GetEntry(j);
      ENSURE_DISTANCE(e, db.GetEnergy(), 0.00001);
      ENSURE_DISTANCE(x, db.GetStartPos().GetX(), 0.00001);
      ENSURE_DISTANCE(y, db.GetStartPos().GetY(), 0.00001);
      ENSURE_DISTANCE(z, db.GetStartPos().GetZ(), 0.00001);
      ENSURE_DISTANCE(len, db.GetLength(), 0.00001);
    }
}
