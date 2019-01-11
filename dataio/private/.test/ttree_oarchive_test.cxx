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

using boost::archive::xml_oarchive;
using boost::archive::text_oarchive;
using boost::archive::binary_oarchive;
using boost::archive::xml_iarchive;
using boost::archive::text_iarchive;
using boost::archive::binary_iarchive;

using boost::serialization::make_nvp;
using boost::serialization::base_object;

#include <TFile.h>
#include <TTree.h>
#include <boost/preprocessor.hpp>

TEST_GROUP(ttree_oarchive_basic);

struct basic
{
  bool b;
  char c;
  unsigned char uc;
  short s;
  unsigned short us;
  int i;
  unsigned int ui;
  int64_t l;
  uint64_t ul;
  float f;
  double d;

  template <class Archive>
  void serialize(Archive& ar, unsigned)
  {
    ar & make_nvp("b", b)
      & make_nvp("c", c)
      & make_nvp("uc", uc)
      & make_nvp("s", s)
      & make_nvp("us", us)
      & make_nvp("i", i)
      & make_nvp("ui", ui)
      & make_nvp("l", l)
      & make_nvp("ul", ul)
      & make_nvp("f", f)
      & make_nvp("d", d);
  }

  basic() 
  {
    b = random_int() % 2;
    c = random_int();
    uc = random_int();
    s = random_int();
    us = random_int();
    i = random_int();
    ui = random_int();
    l = random_int();
    ul = random_int();
    f = random_double();
    d = random_double();
  }
};

I3_SERIALIZABLE(basic);

const string fname = "basic.root";
const unsigned nentries = 50000;

TEST(basic_0)
{
  reseed();
  unlink (fname.c_str());
  TFile tfile(fname.c_str(), "RECREATE");
  TTree ttree("basic", "", 0);
  ttree_oarchive oa(ttree);
  
  for (unsigned i=0; i<nentries; i++)
    {
      {
	basic b;
	oa << make_nvp("basic", b);
      }
    }
  ttree.Write();
  tfile.Close();
}

TEST(basic_1)
{
  reseed();
  TFile tfile(fname.c_str());
  TTree* tree = (TTree*)tfile.Get("basic");
  
  bool b;
  char c;
  unsigned char uc;
  short s;
  unsigned short us;
  int i;
  unsigned int ui;
  int64_t l;
  uint64_t ul;
  float f;
  double d;

  tree->SetBranchAddress("b", &b);
  tree->SetBranchAddress("c", &c);
  tree->SetBranchAddress("uc", &uc);
  tree->SetBranchAddress("s", &s);
  tree->SetBranchAddress("us", &us);
  tree->SetBranchAddress("i", &i);
  tree->SetBranchAddress("ui", &ui);
  tree->SetBranchAddress("l", &l);
  tree->SetBranchAddress("ul", &ul);
  tree->SetBranchAddress("f", &f);
  tree->SetBranchAddress("d", &d);

  ENSURE_EQUAL(tree->GetEntries(), nentries);

  for (unsigned j=0; j<nentries; j++)
    {
      {
	basic b;
	tree->GetEntry(j);
	ENSURE_EQUAL(c, b.c);
	ENSURE_EQUAL(uc, b.uc);
	ENSURE_EQUAL(s, b.s);
	ENSURE_EQUAL(us, b.us);
	ENSURE_EQUAL(i, b.i);
	ENSURE_EQUAL(ui, b.ui);
	ENSURE_EQUAL(l, b.l);
	ENSURE_EQUAL(ul, b.ul);
	ENSURE_DISTANCE(f, b.f, 0.00001);
	ENSURE_DISTANCE(d, b.d, 0.00001);
      }
    }
}
