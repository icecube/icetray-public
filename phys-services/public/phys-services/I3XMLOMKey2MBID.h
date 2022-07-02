/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3XMLOMKey2MBID.h
 * @version $Revision$
 * @date $Date$
 * @author tschmidt
 */
#ifndef I3XMLOMKEY2MBID_H_INCLUDED
#define I3XMLOMKEY2MBID_H_INCLUDED

// forward declarations


// header files

#include <map>
#include <string>

#include <icetray/I3Logging.h>
#include <icetray/I3PointerTypedefs.h>
#include <icetray/OMKey.h>
#include <interfaces/I3OMKey2MBID.h>


// definitions


/**
 * @brief An implementation of I3OMKey2MBID that reads the conversions from a XML file
 */
class I3XMLOMKey2MBID : public I3OMKey2MBID
{
 private:
  typedef std::map<OMKey, long long int> map_omkey2mbid;
  typedef std::map<long long int, OMKey> map_mbid2omkey;

  map_omkey2mbid omkey2mbid_;
  map_mbid2omkey mbid2omkey_;

 public:
  /** Dump conversion table into XML file.
   *
   * \note
   * This XML file can be used as input for I3XMLOMKey2MBID.
   * @param outfile The filename.
   * @param conversionTable The conversion table.
   */
  static void Dump(const std::string& outfile,
                   const std::map<long long int, OMKey>& conversionTable);

  /** Merge a XML file and a conversion table into a new XML file.
   *
   * @param outfile The name of the merged file.
   * @param infile The name of the file that is merged...
   * @param conversionTable ...with this conversion table
   * into <VAR>outfile</VAR>.
   */
  static void Merge(const std::string& outfile,
                    const std::string& infile,
                    const std::map<long long int, OMKey>& conversionTable)
  {
    I3XMLOMKey2MBID tmp = I3XMLOMKey2MBID(infile);
    tmp.mbid2omkey_.insert(conversionTable.begin(), conversionTable.end());
    Dump(outfile, tmp.mbid2omkey_);
  }

  /** Merge two XML files into a new one that store(s) a conversion table.
   *
   * @param outfile The name of the merged file.
   * @param infile1 The name of the file that is merged...
   * @param infile2 ...with this file into <VAR>outfile</VAR>.
   */
  static void Merge(const std::string& outfile,
                    const std::string& infile1,
                    const std::string& infile2)
  {
    Merge(outfile, infile1, I3XMLOMKey2MBID(infile2).mbid2omkey_);
  }


  /** Constructor.
   *
   * @param infile Name of the XML file.
   */
  explicit I3XMLOMKey2MBID(const std::string& infile);
  /** Destructor.
   */
  virtual ~I3XMLOMKey2MBID();
  bool OMKeyExists(long long int mbid) const;
  OMKey GetOMKey(long long int mbid) const;
  bool MBIDExists(OMKey key) const;
  long long int GetMBID(OMKey key) const;

 private:
  // private copy constructors and assignment
  I3XMLOMKey2MBID(const I3XMLOMKey2MBID&);
  I3XMLOMKey2MBID& operator=(const I3XMLOMKey2MBID&);


  // logging
  SET_LOGGER("I3XMLOMKey2MBID");
};

I3_POINTER_TYPEDEFS(I3XMLOMKey2MBID);


inline
bool I3XMLOMKey2MBID::OMKeyExists(long long int mbid) const
{
  return(mbid2omkey_.count(mbid));
}


inline
OMKey I3XMLOMKey2MBID::GetOMKey(long long int mbid) const
{
  std::map<long long int,OMKey>::const_iterator iter = mbid2omkey_.find(mbid);
  if(iter == mbid2omkey_.end())
    log_fatal("unknown mainboard ID - %lld", mbid);

  return(iter->second);
}


inline
bool I3XMLOMKey2MBID::MBIDExists(OMKey key) const
{
  return(omkey2mbid_.count(key));
}


inline
long long int I3XMLOMKey2MBID::GetMBID(OMKey key) const
{
  std::map<OMKey,long long int>::const_iterator iter = omkey2mbid_.find(key);
  if(iter == omkey2mbid_.end())
    log_fatal("unknown OM key - (%d, %u)", key.GetString(), key.GetOM());

  return(iter->second);
}

#endif /*I3XMLOMKEY2MBID_H_INCLUDED*/
