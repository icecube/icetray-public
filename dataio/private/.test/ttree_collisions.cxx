#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>

#include <dataclasses/ttree_oarchive.h>

//#include <icetray/test/random_iarchive.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/is_abstract.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include <boost/random/lagged_fibonacci.hpp>
#include <boost/random/mersenne_twister.hpp>

#include <dataclasses/BoostHeaders.h>

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

TEST_GROUP(ttree_collisions);

struct W 
{
  double d;
  W() : d(random_double()) { }

  template <typename Archive>
  void serialize(Archive & ar, unsigned)
  {
    ar & make_nvp("d", d);
  }
};

I3_SERIALIZABLE(W);

struct X {
  W w;
  template <typename Archive>
  void serialize(Archive & ar, unsigned)
  {
    ar & make_nvp("w", w);
  }
};

I3_SERIALIZABLE(X);

struct Y {
  X x;
  template <typename Archive>
  void serialize(Archive & ar, unsigned)
  {
    ar & make_nvp("x", x);
  }
};

I3_SERIALIZABLE(Y);

struct Z {
  Y y;
  template <typename Archive>
  void serialize(Archive & ar, unsigned)
  {
    ar & make_nvp("y", y);
  }
};

I3_SERIALIZABLE(Z);

struct Top {
  Z z1, z2;
  template <typename Archive>
  void serialize(Archive & ar, unsigned)
  {
    ar & make_nvp("z1", z1);
    ar & make_nvp("z2", z2);
  }
};

I3_SERIALIZABLE(Top);
 
const string fname = "collision.root";
const unsigned nentries = 100;

TEST(collision_0)
{
  reseed();
  unlink (fname.c_str());
  TFile tfile(fname.c_str(), "RECREATE");
  TTree ttree("collision", "", 0);
  ttree_oarchive oa(ttree);
  
  for (unsigned i=0; i<nentries; i++)
    {
      {
	Top top;
	oa << make_nvp("top", top);
      }
    }
  ttree.Write();
  tfile.Close();
}

TEST(collision_1)
{
  reseed();
  TFile tfile(fname.c_str());
  TTree* tree = (TTree*)tfile.Get("collision");

  ENSURE_EQUAL(tree->GetEntries(), nentries);

  for (unsigned j=0; j<nentries; j++)
    {
      {
	Top t;
	double z1, z2;
	tree->SetBranchAddress("z1_d", &z1);
	tree->SetBranchAddress("z2_d", &z2);

	tree->GetEntry(j);

	ENSURE_DISTANCE(z1, t.z1.y.x.w.d, 0.00001);
	ENSURE_DISTANCE(z2, t.z2.y.x.w.d, 0.00001);
      }
    }
}
