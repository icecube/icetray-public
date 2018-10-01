#include <icetray/I3Int.h>
#include <icetray/I3Tray.h>
#include <icetray/I3Frame.h>

#include <I3Test.h>

TEST_GROUP(EvensAndOdds);

// OddCan, as in TrashCan.  Eats odds.
class OddCan : public I3Module
{
public:
  int num_tossed;

  OddCan(const I3Context& context) : I3Module(context) { }

  void Configure()
  { }

  void Physics(I3FramePtr frame)
  {
    log_trace("%s",__PRETTY_FUNCTION__);
    const I3Int& data = frame->Get<I3Int>("myint");
    ENSURE(data.value % 2 != 0);
  }
};

I3_MODULE(OddCan);

// EvenCan, as in TrashCan.  Verifies that it gets an even in the
// frame and eats it.
class EvenCan : public I3Module
{
public:
  int num_tossed;

  EvenCan(const I3Context& context) : I3Module(context) { }

  void Configure()
  { }

  void Physics(I3FramePtr frame)
  {
    log_trace("%s",__PRETTY_FUNCTION__);
    const I3Int& data = frame->Get<I3Int>("myint");
    ENSURE(data.value % 2 == 0);
  }
};
I3_MODULE(EvenCan);


// puts evens in one outbox, odds in the other.
class IntSorter : public I3Module
{
public:
  IntSorter(const I3Context& context) : I3Module(context)
  {
    log_trace("%s",__PRETTY_FUNCTION__);
    AddOutBox("Evens");
    AddOutBox("Odds");
  }

  void Physics(I3FramePtr frame)
  {
    log_trace("%s",__PRETTY_FUNCTION__);
    const I3Int& data = frame->Get<I3Int>("myint");
    if(data.value % 2 == 0)
      PushFrame(frame,"Evens");
    else
      PushFrame(frame,"Odds");
  }
};
I3_MODULE(IntSorter);

TEST(a_evens_and_odds_to_different_modules)
{
  I3Tray tray;
  tray.AddModule("IntGenerator", "generator");
  tray.AddModule<IntSorter>("sorter");
  tray.AddModule<OddCan>("oddcan");
  tray.AddModule<EvenCan>("evencan");

  tray.ConnectBoxes("generator","OutBox","sorter");
  tray.ConnectBoxes("sorter","Evens","evencan");
  tray.ConnectBoxes("sorter","Odds","oddcan");

  tray.Execute(1000);
}

// puts evens in one outbox, odds in the other.
class AddsNullsToFramesWithOddInts : public I3Module
{
public:
  AddsNullsToFramesWithOddInts(const I3Context& context) : I3Module(context)
  {
    log_trace("%s",__PRETTY_FUNCTION__);
  }

  bool ShouldDoPhysics(I3FramePtr frame)
  {
    const I3Int& data = frame->Get<I3Int>("myint");
    return data.value % 2;
  }

  // this should put null ptrs on frames containing odd numbers at
  // myint.value
  void Physics(I3FramePtr frame)
  {
    log_trace("%s",__PRETTY_FUNCTION__);
    frame->Put("oddone", I3FrameObjectPtr());
    PushFrame(frame);
  }
};
I3_MODULE(AddsNullsToFramesWithOddInts);

TEST(b_add_nulls_via_should_do_physics)
{
  std::vector<std::string> odd_musthave_vec, even_mustnothave_vec;
  odd_musthave_vec.push_back("myint");
  odd_musthave_vec.push_back("oddone");
  even_mustnothave_vec.push_back("oddone");

  I3Tray tray;
  tray.AddModule("IntGenerator", "generator");
  tray.AddModule("AddsNullsToFramesWithOddInts", "nulladder");
  tray.AddModule("IntSorter", "sorter");

  tray.AddModule("FrameCheck", "oddcheck")
    ("ensure_physics_has", odd_musthave_vec);

  tray.AddModule("FrameCheck", "evencheck")
    ("ensure_physics_hasnt", even_mustnothave_vec);

  tray.AddModule("CountFrames", "oddcount")
    ("physics", 500);

  tray.AddModule("CountFrames", "evencount")
    ("physics", 500);

  tray.AddModule<OddCan>("oddcan");
  tray.AddModule<EvenCan>("evencan");

  tray.ConnectBoxes("generator","OutBox","nulladder");
  tray.ConnectBoxes("nulladder","OutBox","sorter");
  
  tray.ConnectBoxes("sorter","Evens","evencheck");
  tray.ConnectBoxes("evencheck", "OutBox", "evencount");
  tray.ConnectBoxes("evencount", "OutBox", "evencan");

  tray.ConnectBoxes("sorter","Odds","oddcheck");
  tray.ConnectBoxes("oddcheck", "OutBox", "oddcount");
  tray.ConnectBoxes("oddcount", "OutBox", "oddcan");

  tray.Execute(1000);

}
