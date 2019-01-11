#include <icetray/I3Frame.h>

struct T : I3FrameObject { };

void foo()
{
  I3FramePtr frame;
  boost::shared_ptr<const T> bs = frame->Get<boost::shared_ptr<const T> >("foo");
}
