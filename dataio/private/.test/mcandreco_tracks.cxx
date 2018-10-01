#include <dataclasses/ttree_oarchive.h>

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
#include <dataclasses/I3MCTrack.h>

struct trackpair 
{
  I3ParticlePtr reco, mc;
  template <typename Archive>
  void serialize(Archive &ar, unsigned)
  {
    ar & make_nvp("reco", reco);
    ar & make_nvp("mc", mc);
  }
};

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

TEST_GROUP(two_tracks);

const string fname = "mcandreco.root";
const unsigned nentries = 1000;

static void randomize_db(I3DoubleBang& db)
{
  db.SetStartPos(random_double(), random_double(), random_double());
  db.SetEnergy(random_double());
  db.SetLength(random_double());
}

static void randomize_mc(I3MCTrack& mc)
{
  mc.SetWeight(random_double());
}

TEST(mc_and_reco_0)
{
  reseed();
  unlink (fname.c_str());
  TFile tfile(fname.c_str(), "RECREATE");
  TTree ttree("doublebang", "", 0);
  ttree_oarchive oa(ttree);
  
  trackpair tp;
  I3DoubleBangPtr db(new I3DoubleBang);
  tp.reco = db;
  I3MCTrackPtr mc(new I3MCTrack);
  tp.mc = mc;
  
  for (unsigned i=0; i<nentries; i++)
    {
      randomize_db(*db);
      randomize_mc(*mc);
      oa << make_nvp("mc_and_reco", tp);
    }
  ttree.Write();
  tfile.Close();
}

TEST(mc_and_reco_1)
{
  reseed();
  unlink (fname.c_str());
  TFile tfile(fname.c_str(), "RECREATE");
  TTree ttree("doublebang", "", 0);
  ttree_oarchive oa(ttree);
  
  trackpair tp;
  I3DoubleBangPtr db(new I3DoubleBang);
  tp.reco = db;
  I3DoubleBangPtr mc(new I3DoubleBang);
//  I3MCTrackPtr mc(new I3MCTrack);
  tp.mc = mc;
  
  for (unsigned i=0; i<nentries; i++)
    {
      randomize_db(*db);
      randomize_db(*mc);
//      randomize_mc(*mc);
      oa << make_nvp("mc_and_reco", tp);
    }
  ttree.Write();
  tfile.Close();
}

