/*
 * class: I3Context
 *
 * Version $Id$
 *
 * Date: 29 Sept 2003
 *
 * (c) IceCube Collaboration
 */

#include <icetray/I3Module.h>
#include <icetray/I3Context.h>

I3Context::I3Context()
{
}

I3Context::~I3Context()
{
}

void
I3Context::dump() const
{
  ostringstream oss;
  oss << "[ I3Context @ " << this << "\n";
  for (map_t::const_iterator iter = map_.begin();
       iter != map_.end();
       iter++)
    {
      oss << "   '" << iter->first << " ==> " << I3::name_of(typeid(iter->second)) << "\n";
    }
  oss << "]\n";
  log_trace("%s", oss.str().c_str());
}

