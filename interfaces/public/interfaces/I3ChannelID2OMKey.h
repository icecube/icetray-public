/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id: I3ChannelID2OMKey.h 13154 2005-12-01 21:45:55Z tschmidt $
 *
 * @file I3ChannelID2OMKey.h
 * @version $Revision: 1.10 $
 * @date $Date: 2005-12-01 16:45:55 -0500 (Thu, 01 Dec 2005) $
 * @author tschmidt
 */
#ifndef I3CHANNELID2OMKEY_H_INCLUDED
#define I3CHANNELID2OMKEY_H_INCLUDED

// forward declarations


// header files

#include <map>

#include <icetray/I3DefaultName.h>
#include <icetray/I3PointerTypedefs.h>
#include <icetray/I3Logging.h>
#include <icetray/OMKey.h>

// definitions


/**
 * @brief This is an icetray service which provides conversions
 * between channel ID and OM key.
 */
class I3ChannelID2OMKey
{
 public:
  /** 
   * Default constructor.
   */
  I3ChannelID2OMKey();
  /** 
   * Destructor.
   */
  virtual ~I3ChannelID2OMKey() = 0;
  /** 
   * Checks whether the specified channel ID exists in the table.
   * 
   * @param channelID Channel ID.
   */
  virtual bool ChannelIDExists(int channelID) const;
  /** 
   * Checks whether the specified OM key exists in the table.
   * 
   * @param key OM key.
   */
  virtual bool OMKeyExists(OMKey key) const;
  /** 
   * Returns the OM key associated with the specified channel ID.
   * 
   * @param channelID Channel ID.
   */
  virtual OMKey GetOMKey(int channelID) const;
  /** 
   * Returns the channel ID associated with the specified OM key.
   * 
   * @param key OM key.
   */
  virtual int GetChannelID(OMKey key) const;

 protected:
  virtual void InsertIntoOMKey2ChannelIDMapping(OMKey key, int channelID);
  virtual void InsertIntoChannelID2OMKeyMapping(int key, OMKey omKey);
  virtual void ResetOMKey2ChannelIDMapping();
  virtual void ResetChannelID2OMKeyMapping();

 private:
  std::map<OMKey,int> omkey2channelid_;
  std::map<int,OMKey> channelid2omkey_;


  // private copy constructors and assignment
  I3ChannelID2OMKey(const I3ChannelID2OMKey& rhs); 
  I3ChannelID2OMKey& operator=(const I3ChannelID2OMKey& rhs); 


  // logging
  SET_LOGGER("I3ChannelID2OMKey");
};

/**
 * pointer type to insulate users from memory management
 */
I3_POINTER_TYPEDEFS(I3ChannelID2OMKey);

/**
 * define a default name to address service in a context
 */
I3_DEFAULT_NAME(I3ChannelID2OMKey);


inline
bool I3ChannelID2OMKey::ChannelIDExists(int channelID) const
{
  return(channelid2omkey_.count(channelID));
}


inline
bool I3ChannelID2OMKey::OMKeyExists(OMKey key) const
{
  return(omkey2channelid_.count(key));
}


inline
OMKey I3ChannelID2OMKey::GetOMKey(int channelID) const
{
  std::map<int,OMKey>::const_iterator found = channelid2omkey_.find(channelID);
  if(found == channelid2omkey_.end())
    log_fatal("unknown channel ID - %d", channelID);
  return(found->second);
}


inline
int I3ChannelID2OMKey::GetChannelID(OMKey key) const
{
  std::map<OMKey,int>::const_iterator found = omkey2channelid_.find(key);
  if(found == omkey2channelid_.end())
    log_fatal("unknown OM key - (%d, %u)", key.GetString(), key.GetOM());
  return(found->second);
}

#endif /*I3CHANNELID2OMKEY_H_INCLUDED*/
