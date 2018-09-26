#include <I3Test.h>

#include <dataclasses/physics/I3FilterResult.h>

#include <fstream>
#include <archive/xml_oarchive.hpp>
#include <boost/filesystem.hpp>

TEST_GROUP(I3FilterResultTest);

TEST(out_and_in)
{
  const char *fname = "test-filtermask-out.xml";
  {
    std::ofstream fout(fname);
    icecube::archive::xml_oarchive ar(fout);
    I3FilterResult result;
    ENSURE(!result.conditionPassed,"Initialized correctly");
    ENSURE(!result.prescalePassed,"Initialized correctly");
    result.conditionPassed = true;
    result.prescalePassed = true;
    ar << make_nvp("result",result);
  }

  {
    std::ifstream fin(fname);
    icecube::archive::xml_iarchive ar(fin);
    I3FilterResult result;
    ar >> make_nvp("result",result);
    ENSURE(result.conditionPassed,"Read In correctly");
    ENSURE(result.prescalePassed,"Read In correctly");
  }

  boost::filesystem::remove(fname);
}
