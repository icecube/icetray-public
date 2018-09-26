/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3TriggerHierarchy.cxx
 * @version $Revision$
 * @date $Date$
 */
#include <icetray/serialization.h>
#include <dataclasses/physics/I3TriggerHierarchy.h>

#include <iostream>

std::string DumpChildren(const I3TriggerHierarchy& t,
		    I3TriggerHierarchy::iterator i){
  std::stringstream s;
  I3TriggerHierarchy::sibling_iterator si;
  for(si = t.begin(i); si != t.end(i); si++){
    for(int j=0; j<2*t.depth(si); j++) s<<" ";
    s<< *si << std::endl;
    s<<DumpChildren(t,si);
  }
  return s.str();
}

std::ostream& operator<<(std::ostream& oss, const I3TriggerHierarchy& t){
   I3TriggerHierarchy::sibling_iterator i;
  oss<<"*** TRIGGER DUMP - BEGIN***"<<std::endl;
  for(i = t.begin(); i != t.end(); i++){
    for(int j=0; j<2*t.depth(i); j++) oss<<" ";
     oss<< *i  << std::endl;
    oss<<DumpChildren(t,i);
  }
  oss<<"*** TRIGGER DUMP - END***"<<std::endl;
  return oss;
}
I3_SERIALIZABLE(I3TriggerHierarchy);
