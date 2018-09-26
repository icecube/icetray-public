/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3TriggerHierarchy.h
 * @version $Revision$
 * @date $Date$
 * @author tschmidt
 */
#ifndef I3_TRIGGER_HIERARCHY_H_INCLUDED
#define I3_TRIGGER_HIERARCHY_H_INCLUDED

#include <algorithm>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

#include "dataclasses/I3Tree.h"
#include "dataclasses/physics/I3Trigger.h"
#include "icetray/I3DefaultName.h"

/**
 * Type definition for a n-ary tree of I3Trigger.
 * 
 * In contrast to AMANDA, there is no "flat" structure of coincident triggers
 * in IceCube anymore. DAQ provides a more complicated n-ary tree-like hierarchy
 * of triggers instead. So called "global triggers" hold "subdetector triggers"
 * (child nodes of the corresponding node within the tree).
 * "Global triggers" might be assigned as childs to an additional "global trigger"
 * of type "MERGED", if two or more of these overlap in time.
 * 
 * @image html aTriggerHierarchy.jpg
 * 
 * Triggering at "string processor" level will add one additional level of
 * siblings in future.
 * 
 * A n-ary tree of I3Trigger describes both scenarios very well.
 * One might only use top level siblings for triggers in AMANDA and a full
 * tree-like hierarchy for triggers in IceCube.
 */
typedef I3Tree<I3Trigger> I3TriggerHierarchy;

/**
 * pointer type to insulate users from memory management
 */

std::ostream& operator<<(std::ostream& oss, const I3TriggerHierarchy& t);

I3_POINTER_TYPEDEFS(I3TriggerHierarchy);

/*
 * Specialize I3Frame::Get() to turn convert various objects
 * in the frame into I3TriggerHierarchies.
 */

// need to hide this from ROOT
#ifndef __CINT__
#include "icetray/I3Frame.h"

template <>
I3TriggerHierarchyConstPtr
I3Frame::Get(const std::string& name, void*, void*) const;
#endif //__CINT__

/**
 * define a default name to address hierarchy in a frame
 */
I3_DEFAULT_NAME(I3TriggerHierarchy);

namespace I3TriggerHierarchyUtils
{
  namespace internal
  {
    inline bool EqualSrc(const I3Trigger& trigger, TriggerKey::SourceID src)
    { return trigger.GetTriggerKey().GetSource() == src; }

    inline bool EqualType(const I3Trigger& trigger, TriggerKey::TypeID type)
    { return trigger.GetTriggerKey().GetType() == type; }

    inline bool EqualSrcType(const I3Trigger& trigger,
                             TriggerKey::SourceID src, TriggerKey::TypeID type)
    { return (trigger.GetTriggerKey().GetSource() == src)
              && (trigger.GetTriggerKey().GetType() == type); }

    inline bool EqualKey(const I3Trigger& trigger, const TriggerKey& key)
    { return trigger.GetTriggerKey() == key; }
  }
  
  inline size_t
  Count(const I3TriggerHierarchy& triggers,
        TriggerKey::SourceID src)
  {
    return std::count_if(triggers.begin(), triggers.end(),
                    boost::lambda::bind(internal::EqualSrc,
                                        boost::lambda::_1, src));
  }

  inline size_t
  Count(const I3TriggerHierarchy& triggers,
        TriggerKey::TypeID type)
  {
    return std::count_if(triggers.begin(), triggers.end(),
                    boost::lambda::bind(internal::EqualType,
                                        boost::lambda::_1, type));
  }

  inline size_t
  Count(const I3TriggerHierarchy& triggers,
        TriggerKey::SourceID src, TriggerKey::TypeID type)
  {
    return std::count_if(triggers.begin(), triggers.end(),
                    boost::lambda::bind(internal::EqualSrcType,
                                        boost::lambda::_1, src, type));
  }

  inline size_t
  Count(const I3TriggerHierarchy& triggers,
        const TriggerKey& key)
  {
    return std::count_if(triggers.begin(), triggers.end(),
                    boost::lambda::bind(internal::EqualKey,
                                        boost::lambda::_1, key));
  }

  inline I3TriggerHierarchy::iterator
  Find(const I3TriggerHierarchy& triggers,
       TriggerKey::SourceID src,
       I3TriggerHierarchy::iterator iter = I3TriggerHierarchy::iterator())
  {
    return std::find_if(!triggers.is_valid(iter) ? triggers.begin() : iter, triggers.end(),
                   boost::lambda::bind(internal::EqualSrc,
                                       boost::lambda::_1, src));
  }

  inline I3TriggerHierarchy::iterator
  Find(const I3TriggerHierarchy& triggers,
       TriggerKey::TypeID type,
       I3TriggerHierarchy::iterator iter = I3TriggerHierarchy::iterator())
  {
    return std::find_if(!triggers.is_valid(iter) ? triggers.begin() : iter, triggers.end(),
                   boost::lambda::bind(internal::EqualType,
                                       boost::lambda::_1, type));
  }

  inline I3TriggerHierarchy::iterator
  Find(const I3TriggerHierarchy& triggers,
       TriggerKey::SourceID src, TriggerKey::TypeID type,
       I3TriggerHierarchy::iterator iter = I3TriggerHierarchy::iterator())
  {
    return std::find_if(!triggers.is_valid(iter) ? triggers.begin() : iter, triggers.end(),
                   boost::lambda::bind(internal::EqualSrcType,
                                       boost::lambda::_1, src, type));
  }

  inline I3TriggerHierarchy::iterator
  Find(const I3TriggerHierarchy& triggers,
       const TriggerKey& key,
       I3TriggerHierarchy::iterator iter = I3TriggerHierarchy::iterator())
  {
    return std::find_if(!triggers.is_valid(iter) ? triggers.begin() : iter, triggers.end(),
                   boost::lambda::bind(internal::EqualKey,
                                       boost::lambda::_1, key));
  }

  inline I3TriggerHierarchy 
  Insert(I3TriggerHierarchy& triggers, const I3Trigger& trig) {
    triggers.insert(triggers.begin(), trig);    
    return triggers;
  }
}


#endif


