#include <I3Test.h>

#include "icetray/I3Frame.h"
#include <icetray/I3DefaultName.h>

TEST_GROUP(DefaultFrameNames);

struct TestedData : public I3FrameObject
{
  int data;
};

typedef boost::shared_ptr<TestedData> TestedDataPtr;
typedef boost::shared_ptr<const TestedData> TestedDataConstPtr;

I3_DEFAULT_NAME(TestedData);

TEST(default_name_ref)
{
  I3Frame frame(I3Frame::None);

  TestedDataPtr tested(new TestedData);
  tested->data = 56;

  frame.Put(tested);

  const TestedData& testedOut = frame.Get<TestedData>();
  ENSURE(testedOut.data==56,"Data same when it comes out");
}

TEST(default_name_ptr)
{
  I3Frame frame(I3Frame::None);

  TestedDataPtr tested(new TestedData);
  tested->data = 56;

  frame.Put(tested);

  TestedDataConstPtr testedOut = frame.Get<TestedDataConstPtr>();
  ENSURE((bool)testedOut);
  ENSURE(testedOut->data==56,"Data same when it comes out");
}

TEST(testing_const_cast_ptr)
{
  I3Frame frame(I3Frame::None);

  TestedDataPtr tested(new TestedData);
  tested->data = 56;

  frame.Put(tested);

  TestedDataConstPtr testedOut = frame.Get<TestedDataConstPtr>();
  ENSURE((bool)testedOut);
  ENSURE(testedOut->data==56,"Data same when it comes out");
  TestedDataPtr unconst = boost::const_pointer_cast<TestedData>(testedOut);
  unconst->data = 90;
}

TEST(illegal_key_with_space)
{
  I3Frame frame(I3Frame::None);

  TestedDataPtr tested(new TestedData);
  tested->data = 56;

  try {
    frame.Put("bogus name", tested);
    FAIL("frame should reject keys containing spaces");
  } catch (const std::exception& e) { }
}

TEST(illegal_key_with_tab)
{
  I3Frame frame(I3Frame::None);

  TestedDataPtr tested(new TestedData);
  tested->data = 56;

  try {
    frame.Put("bogus\tname", tested);
    FAIL("frame should reject keys containing tab");
  } catch (const std::exception& e) { }
}

TEST(illegal_key_with_newline)
{
  I3Frame frame(I3Frame::None);

  TestedDataPtr tested(new TestedData);
  tested->data = 56;

  try {
    frame.Put("bogus\nname", tested);
    FAIL("frame should reject keys containing newline");
  } catch (const std::exception& e) { }
}

TEST(double_put_is_rejected)
{
  I3Frame frame(I3Frame::None);

  TestedDataPtr tested(new TestedData);
  tested->data = 56;

  frame.Put("somewhere", tested);
  try {
    frame.Put("somewhere", tested);
    FAIL("frame should reject attempt to put something in an already full slot");
  } catch (const std::exception& e) { }

}
