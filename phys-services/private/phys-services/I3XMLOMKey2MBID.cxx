/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file phys-services/I3XMLOMKey2MBID.cxx
 * @brief implementation of the I3XMLOMKey2MBID class
 * @version $Revision$
 * @date $Date$
 * @author tschmidt
 */

// class header files

#include <phys-services/I3XMLOMKey2MBID.h>
#include <icetray/open.h>

#include <utility>
#include <archive/xml_iarchive.hpp>
#include <archive/xml_oarchive.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <serialization/map.hpp>
#include <serialization/utility.hpp>

// definitions


// namespace declarations

using namespace std;

// implementation

void I3XMLOMKey2MBID::Dump(const string& outfile,
                           const map<long long int, OMKey>& conversionTable)
{
  if(outfile.empty()) log_fatal("undefined file name");

  boost::iostreams::filtering_ostream ofs;
  I3::dataio::open(ofs, outfile);
  icecube::archive::xml_oarchive oa(ofs);

  oa << icecube::serialization::make_nvp("MBID_OMKey", conversionTable);
}


I3XMLOMKey2MBID::I3XMLOMKey2MBID(const string& infile)
  : I3OMKey2MBID()
{
  if(infile.empty()) log_fatal("undefined file name");
  if(!boost::filesystem::exists(infile)
     || boost::filesystem::is_directory(infile))
    log_fatal("cannot find file \"%s\"", infile.c_str());

  boost::iostreams::filtering_istream ifs;
  I3::dataio::open(ifs, infile);

  if(!ifs.good()) log_fatal("cannot open file \"%s\"", infile.c_str());
  icecube::archive::xml_iarchive ia(ifs);

  ia >> icecube::serialization::make_nvp("MBID_OMKey", mbid2omkey_);

  map<long long int, OMKey>::const_iterator iter;
  for(iter = mbid2omkey_.begin(); iter != mbid2omkey_.end(); ++iter)
    if(!(omkey2mbid_.insert(make_pair(iter->second, iter->first))).second)
      log_fatal("duplicated OM key - (%d, %u)",
                iter->second.GetString(), iter->second.GetOM());
}


I3XMLOMKey2MBID::~I3XMLOMKey2MBID()
{
}
