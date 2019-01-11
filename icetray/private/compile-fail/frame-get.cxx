#include <icetray/I3Frame.h>

struct T : I3FrameObject { };

void foo()
{
  I3FramePtr frame;
  frame->Get<boost::shared_ptr<T> >("foo");
}
