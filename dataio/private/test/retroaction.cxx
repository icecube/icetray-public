/**
   copyright  (C) 2004
   the icecube collaboration
   $Id: HasBool.cxx 7856 2005-05-20 17:43:47Z olivas $

   @version $Revision: 1.2 $
   @date $Date: 2005-05-20 19:43:47 +0200 (Fri, 20 May 2005) $

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
  const char* src = getenv("I3_SRC");
  if (! src)
    log_fatal("can't find data files w/o I3_SRC set (load your env-shell.sh)");
  
  string ports = src;
  glob((ports + "/dataio/resources/data/serialization/*/*.i3").c_str(), i3files);
  ENSURE(i3files.size() != 0);
  BOOST_FOREACH(const string& s, i3files)
    {
      log_info("%s", s.c_str());
      I3FramePtr fp = load_i3_file(s);
      ENSURE(fp);
	
      cout << "From " << s << ":\n" << *fp << "\n";
      for (I3Frame::const_iterator iter = fp->begin();
	   iter != fp->end();
	   iter++)
	cout << iter->first << " deserialized\n";
    }
}
