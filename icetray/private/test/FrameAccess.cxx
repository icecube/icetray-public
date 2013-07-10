#include <I3Test.h>

#include "icetray/I3Frame.h"
#include "icetray/I3Module.h"
#include "icetray/I3TrayHeaders.h"

TEST_GROUP(I3Frame);

struct Grandpa : I3FrameObject { };

typedef boost::shared_ptr<Grandpa> GrandpaPtr;
typedef boost::shared_ptr<const Grandpa> GrandpaConstPtr;

struct Pa : Grandpa { };
typedef boost::shared_ptr<Pa> PaPtr;
typedef boost::shared_ptr<const Pa> PaConstPtr;

struct Son : Pa { };
typedef boost::shared_ptr<Son> SonPtr;
typedef boost::shared_ptr<const Son> SonConstPtr;

struct Daughter : Pa { };
typedef boost::shared_ptr<Daughter> DaughterPtr;
typedef boost::shared_ptr<const Daughter> DaughterConstPtr;

TEST(test)
{
  I3Frame frame(I3Frame::None);
    
  shared_ptr<Grandpa> grandpa_ptr(new Grandpa);
  shared_ptr<Grandpa> pa_ptr(new Pa);      
  shared_ptr<Grandpa> son_ptr(new Son);
  shared_ptr<Grandpa> daughter_ptr(new Daughter);

  ENSURE(!frame.count("grandpa"));
  ENSURE(!frame.count("pa"));
  ENSURE(!frame.count("son"));
  ENSURE(!frame.count("daughter"));

  frame.Put("grandpa",grandpa_ptr);
  frame.Put("pa",pa_ptr);
  frame.Put("son",son_ptr);
  try {
    frame.Put("son",son_ptr);
    FAIL("that should have thrown");
  } catch (const std::exception& e) {
    // ok here
  }
  frame.Put("daughter",daughter_ptr);

  ENSURE((bool)frame.Get<GrandpaConstPtr>("grandpa"));
  const Grandpa& grandpa = frame.Get<Grandpa>("grandpa");
  ENSURE(&grandpa);

  ENSURE((bool)frame.Get<PaConstPtr>("pa"));
  try {
    const Pa& pa = frame.Get<Pa>("grandpa");
    std::cout << "got pa @ " << &pa;
    FAIL("that should have thrown");
  } catch (const std::exception& e) {
    // ok
  }
  ENSURE((bool)frame.Get<SonConstPtr>("son"));
  ENSURE((bool)frame.Get<DaughterConstPtr>("daughter"));
    
  ENSURE((bool)frame.Get<PaConstPtr>("son"));
  frame.Get<Pa>("son");
  frame.Get<Grandpa>("son");

  ENSURE((bool)frame.Get<GrandpaConstPtr>("son"));

  const Grandpa& gp = frame.Get<Daughter>("daughter");
  ENSURE(&gp);

  try {
    frame.Get<Daughter>("son");
    FAIL("that should have thrown");
  } catch (const std::exception& e) {
    log_debug("yep, threw.");
  }
    
  ENSURE(! frame.Get<DaughterConstPtr>("son"));
  frame.Dump();

  ENSURE(! frame.Get<GrandpaConstPtr>("not_there"));

}
