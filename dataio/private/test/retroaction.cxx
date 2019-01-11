/**
   copyright  (C) 2004
   the icecube collaboration
   $Id: retroaction.cxx 165886 2018-10-01 14:37:58Z nwhitehorn $

   @version $Revision: 165886 $
   @date $Date: 2018-10-01 07:37:58 -0700 (Mon, 01 Oct 2018) $

*/

#include <I3Test.h>
#include <fstream>
#include <icetray/serialization.h>

#include <icetray/I3Logging.h>
#include <icetray/Utility.h>

#include "serialization-test.h"

#include <boost/preprocessor.hpp>
#include <boost/foreach.hpp>

using namespace boost::archive;
using namespace std;

TEST_GROUP(retroaction);

TEST(read)
{
  vector<string> i3files;
  string ports = getenv("I3_SRC");
  glob((ports + "/dataio/resources/data/serialization/*/*.i3").c_str(), i3files);
  ENSURE(i3files.size() != 0);
  BOOST_FOREACH(const string& s, i3files)
    {
      log_info("%s", s.c_str());
      I3FramePtr fp = load_i3_file(s);
      ENSURE(fp);
      cout << "From " << s << ":\n" << *fp << "\n";
    }
}
