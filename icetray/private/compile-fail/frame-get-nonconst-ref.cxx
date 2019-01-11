#include <icetray/I3Frame.h>

struct T : I3FrameObject { };

void foo()
{
  I3FramePtr frame;
  T& t = frame->Get<T>("foo");
}
