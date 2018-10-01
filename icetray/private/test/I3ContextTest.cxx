/**
   copyright  (C) 2004
   the icecube collaboration
   $Id$

   @version $Revision$
   @date $Date$
   @author troy d. straszheim <troy@resophonic.com>
*/

#include <I3Test.h>
#include <icetray/IcetrayFwd.h>
#include <icetray/I3Logging.h>
#include <icetray/I3Context.h>

#include <string>
using namespace std;

TEST_GROUP(I3ContextAbuse);

struct Cc
{
    
};

struct Dd
{

};

I3_DEFAULT_NAME(Cc);
I3_POINTER_TYPEDEFS(Cc);

I3_DEFAULT_NAME(Dd);
I3_POINTER_TYPEDEFS(Dd);

#define THROWS(CMD) try { CMD; FAIL("that should have thrown"); }	\
  catch (const std::exception &e) { /* good. */ }

TEST(has)
{
  I3Context c;

  ENSURE(!c.Has<bool>("Cc"));
  ENSURE(!c.Has<bool>("Dd"));

  ENSURE(!c.Has<Cc>("Cc"));
  ENSURE(!c.Has<CcPtr>("Cc"));
  ENSURE(!c.Has<CcConstPtr>("Cc"));
  ENSURE(!c.Has<Dd>("Cc"));
  ENSURE(!c.Has<DdPtr>("Cc"));
  ENSURE(!c.Has<DdConstPtr>("Cc"));

  ENSURE(!c.Has<Cc>("Dd"));
  ENSURE(!c.Has<CcPtr>("Dd"));
  ENSURE(!c.Has<CcConstPtr>("Dd"));
  ENSURE(!c.Has<Dd>("Dd"));
  ENSURE(!c.Has<DdPtr>("Dd"));
  ENSURE(!c.Has<DdConstPtr>("Dd"));

  ENSURE(!c.Has<Cc>());
  ENSURE(!c.Has<CcPtr>());
  ENSURE(!c.Has<CcConstPtr>());
  ENSURE(!c.Has<Dd>());
  ENSURE(!c.Has<DdPtr>());
  ENSURE(!c.Has<DdConstPtr>());

  THROWS(c.Get<Cc>());
  THROWS(c.Get<Dd>());
  THROWS(c.Get<bool>("boolio"));

  CcPtr spc(new Cc);
  c.Put(spc);

  ENSURE(!c.Has<bool>("Cc"));
  ENSURE(!c.Has<bool>("Dd"));

  ENSURE(c.Has<Cc>("Cc"));
  ENSURE(c.Has<CcPtr>("Cc"));
  ENSURE(!c.Has<CcConstPtr>("Cc"));
  ENSURE(c.Has<Cc>());
  ENSURE(c.Has<CcPtr>());
  ENSURE(!c.Has<CcConstPtr>());

  ENSURE(!c.Has<Dd>("Cc"));
  ENSURE(!c.Has<DdPtr>("Cc"));
  ENSURE(!c.Has<DdConstPtr>("Cc"));

  ENSURE(!c.Has<Cc>("Dd"));
  ENSURE(!c.Has<Dd>("Dd"));

  ENSURE(c.Has<Cc>());
  ENSURE(!c.Has<Dd>());

  // if you put it const, you get it const, and no other way.
  CcConstPtr spcc(new Cc);
  c.Put(spcc, "constc");
  
  ENSURE(!c.Has<Cc>("constc"));
  ENSURE(c.Has<const Cc>("constc"));
  ENSURE(!c.Has<CcPtr>("constc"));
  ENSURE(c.Has<CcConstPtr>("constc"));

  THROWS(c.Get<Cc>("constc"));
  c.Get<const Cc>("constc"); // doesnt throw
  ENSURE(!c.Get<CcPtr>("constc")); // returns null.  
  ENSURE((bool)c.Get<CcConstPtr>("constc")); // success

  ENSURE(!c.Has<Dd>("dplace"));
  ENSURE(!c.Has<const Dd>("dplace"));
  ENSURE(!c.Has<DdPtr>("dplace"));
  ENSURE(!c.Has<DdConstPtr>("dplace"));
  ENSURE(!c.Has<const DdPtr>("dplace"));
  ENSURE(!c.Has<const DdConstPtr>("dplace"));

  const DdConstPtr dcpp(new Dd);
  c.Put(dcpp, "dplace");
  ENSURE(!c.Has<Dd>("dplace"));
  ENSURE(c.Has<const Dd>("dplace"));
  ENSURE(!c.Has<DdPtr>("dplace"));
  ENSURE(c.Has<DdConstPtr>("dplace"));
  ENSURE(!c.Has<const DdPtr>("dplace"));
  ENSURE(c.Has<const DdConstPtr>("dplace"));

  c.Get<Cc>();
  THROWS(c.Get<Dd>());
  THROWS(c.Get<bool>("boolio"));

  DdPtr spd(new Dd);
  c.Put(spd);

  ENSURE(!c.Has<bool>("Cc"));
  ENSURE(!c.Has<bool>("Dd"));

  ENSURE(c.Has<Cc>("Cc"));
  ENSURE(!c.Has<Dd>("Cc"));

  ENSURE(!c.Has<Cc>("Dd"));
  ENSURE(c.Has<Dd>("Dd"));

  ENSURE(c.Has<Cc>());
  ENSURE(c.Has<Dd>());
  ENSURE(c.Has<DdPtr>());
  ENSURE(!c.Has<DdConstPtr>());
  ENSURE(!c.Has<const DdConstPtr>());
  ENSURE((bool)c.Get<DdPtr>());
  ENSURE(!c.Get<DdConstPtr>());

  c.Get<Cc>();
  c.Get<Dd>();
  THROWS(c.Get<bool>("boolio"));
  c.Put(spd, "elsewhere");

  ENSURE(!c.Has<bool>("Cc"));
  ENSURE(!c.Has<bool>("Dd"));

  ENSURE(c.Has<Cc>("Cc"));
  ENSURE(!c.Has<Dd>("Cc"));

  ENSURE(!c.Has<Cc>("Dd"));
  ENSURE(c.Has<Dd>("Dd"));

  ENSURE(c.Has<Cc>());
  ENSURE(c.Has<Dd>());
  ENSURE(c.Has<Dd>("elsewhere"));

  c.Get<Cc>();
  c.Get<Dd>();
  c.Get<Dd>("elsewhere");
  THROWS(c.Get<Cc>("elsewhere"));
  THROWS(c.Get<bool>("boolio"));

  boost::shared_ptr<bool> spb(new bool);
  
  c.Put(spb, "boolio");
  ENSURE(c.Has<bool>("boolio"));

  c.Get<Cc>();
  c.Get<Dd>();
  c.Get<Dd>("elsewhere");
  THROWS(c.Get<Cc>("elsewhere"));
  c.Get<bool>("boolio");

}

