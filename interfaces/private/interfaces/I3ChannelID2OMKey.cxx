/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id: I3ChannelID2OMKey.cxx 7629 2005-05-15 15:35:25Z tschmidt $
 *
 * @file I3ChannelID2OMKey.cxx
 * @brief implementation of the class I3ChannelID2OMKey
 * @version $Revision: $
 * @date $Date: 2005-05-15 17:35:25 +0200 (Sun, 15 May 2005) $
 * @author tschmidt
 */

// class header file

#include <interfaces/I3ChannelID2OMKey.h>

#include <utility>

// namespace declarations

using namespace std;

// implementation

I3ChannelID2OMKey::I3ChannelID2OMKey()
{
}


I3ChannelID2OMKey::~I3ChannelID2OMKey()
{
}


void I3ChannelID2OMKey::InsertIntoOMKey2ChannelIDMapping(OMKey key, int channelID)
{
  if(!omkey2channelid_.insert(make_pair(key, channelID)).second)
    log_fatal("duplicated OM key - (%d, %u)", key.GetString(), key.GetOM());
}


void I3ChannelID2OMKey::InsertIntoChannelID2OMKeyMapping(int key, OMKey omKey)
{
  if(!channelid2omkey_.insert(make_pair(key, omKey)).second)
    log_fatal("duplicated channel ID - %d", key);
}


void I3ChannelID2OMKey::ResetOMKey2ChannelIDMapping()
{
  omkey2channelid_.clear();
}


void I3ChannelID2OMKey::ResetChannelID2OMKeyMapping()
{
  channelid2omkey_.clear();
}
