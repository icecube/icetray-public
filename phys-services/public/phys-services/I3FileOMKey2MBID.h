/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3FileOMKey2MBID.h
 * @version $Revision:$
 * @date $Date$
 * @author pretz
 */

#ifndef I3FILEOMKEY2MBID_H_INCLUDED
#define I3FILEOMKEY2MBID_H_INCLUDED

#include <map>
#include <string>

#include <icetray/I3Logging.h>
#include <icetray/I3PointerTypedefs.h>
#include <interfaces/I3OMKey2MBID.h>

#include <boost/iterator/transform_iterator.hpp>
#include <boost/type_traits/remove_const.hpp>

/**
 * @brief An implementation of I3OMKey2MBID that reads the conversions from a file.
 */
class I3FileOMKey2MBID : public I3OMKey2MBID
{
 private:
  typedef std::map<OMKey,long long int> map_omkey2mbid;
  typedef std::map<long long int,OMKey> map_mbid2omkey;
  
  map_omkey2mbid omkey2mbid_;
  map_mbid2omkey mbid2omkey_;
  
 public:
  /** Constructor takes the name of the input file.
   */
  explicit I3FileOMKey2MBID(const std::string& infile);
  /** Destructor.
   */
  virtual ~I3FileOMKey2MBID();
  bool OMKeyExists(long long int mbid) const;
  OMKey GetOMKey(long long int mbid) const;
  bool MBIDExists(OMKey key) const;
  long long int GetMBID(OMKey key) const;
  /** Dump conversion table into XML file.
   * 
   * \note
   * This XML file can be used as input for I3XMLOMKey2MBID.
   * @param outfile The filename.
   */
  virtual void DumpAsXML(const std::string& outfile) const;

 private:
  OMKey OMKeyize(const std::string& key);


  // private copy constructors and assignment
  I3FileOMKey2MBID(const I3FileOMKey2MBID&);
  I3FileOMKey2MBID& operator=(const I3FileOMKey2MBID&);


  // logging
  SET_LOGGER("I3FileOMKey2MBID");
};

I3_POINTER_TYPEDEFS(I3FileOMKey2MBID);


inline
bool I3FileOMKey2MBID::OMKeyExists(long long int mbid) const
{
  return(mbid2omkey_.count(mbid));
}


inline
OMKey I3FileOMKey2MBID::GetOMKey(long long int mbid) const
{
  std::map<long long int,OMKey>::const_iterator iter = mbid2omkey_.find(mbid);
  if(iter == mbid2omkey_.end())
    log_fatal("unknown mainboard ID - %lld", mbid);

  return(iter->second);
}


inline
bool I3FileOMKey2MBID::MBIDExists(OMKey key) const
{
  return(omkey2mbid_.count(key));
}


inline
long long int I3FileOMKey2MBID::GetMBID(OMKey key)  const
{
  std::map<OMKey,long long int>::const_iterator iter = omkey2mbid_.find(key);
  if(iter == omkey2mbid_.end())
    log_fatal("unknown OM key - (%d, %u)", key.GetString(), key.GetOM());

  return(iter->second);
}

#endif /*I3FILEOMKEY2MBID_H_INCLUDED*/
