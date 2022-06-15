/**
 *
 *  copyright (C) 2004, 2005, 2006, 2007 Troy D. Straszheim
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *
 *  SPDX-License-Identifier: BSD-2-Clause
 *
 */

#include <I3Test.h>
#include <icetray/IcetrayFwd.h>
#include <icetray/I3Logging.h>
#define I3_I3FRAME_TESTING
#include <icetray/I3Frame.h>

#include <icetray/I3Int.h>
#include <icetray/I3FrameObject.h>
#include <icetray/serialization.h>
#include <icetray/open.h>
#include <string>
#include <fstream>

#include <boost/iostreams/filtering_stream.hpp>
namespace io = boost::iostreams;

using namespace std;

TEST_GROUP(I3Frame);

//
// saves a frame and loads it again to reset the internal buffers
//
I3FramePtr saveload(I3Frame& f, const std::string& filename = "frame.i3")
{
  unlink(filename.c_str());
  {
    io::filtering_ostream fos;
    I3::dataio::open(fos, filename);
    f.save(fos);
    fos.flush();
  }
  I3FramePtr rv(new I3Frame);
  rv->drop_blobs(false);

  io::filtering_istream fis;
  I3::dataio::open(fis, filename);
  bool r = rv->load(fis);
  if (!r)
    log_fatal("egh");

  remove(filename.c_str());
  return rv;
}

TEST(Get)
{
  I3Frame f;
  //  TPtr ptr(new T);
  I3IntPtr ptr(new I3Int);
  ptr->value = 133132;
  ENSURE(!f.Has("t"));
  f.Put("t", ptr);
  ENSURE(f.Has("t"));

  ENSURE(!f.has_blob("t"));
  ENSURE(f.has_ptr("t"));
  ENSURE((bool)f.Get<I3IntConstPtr>("t"));
  I3IntConstPtr ip = f.Get<I3IntConstPtr>("t");
  ENSURE_EQUAL(ip->value, ptr->value);
  ENSURE_EQUAL(ip.get(), ptr.get());
}

TEST(GetStop)
{
	I3Frame f(I3Frame::DAQ);
	I3IntPtr ptr(new I3Int);
	ptr->value = 110101;
	f.Put("f", ptr);
	ENSURE_EQUAL(f.GetStop("f"), I3Frame::DAQ);
	EXPECT_THROW(f.GetStop("doesnotexist"), "This should throw.");
}

TEST(Replace)
{
  I3Frame f(I3Frame::DAQ);
  I3IntPtr i1(new I3Int(7));
  f.Put("MyInt",i1);
  ENSURE_EQUAL(f.Get<I3Int>("MyInt"), *i1);
  I3IntPtr i2(new I3Int(22));
  f.Replace("MyInt",i2);
  ENSURE_EQUAL(f.Get<I3Int>("MyInt"), *i2);
  ENSURE_EQUAL(f.GetStop("MyInt"), I3Frame::DAQ);
}

TEST(ReplaceNotFound)
{
  I3Frame f(I3Frame::DAQ);
  I3IntPtr i2(new I3Int(22));
  try{
    f.Replace("MyInt",i2);
    FAIL("I3Frame::Replace should fail on a nonexistant key");
  }catch(...){
    //expect an exception. do nothing.
  }
}

TEST(const_iterator_begin)
{
  I3Frame f;
  I3IntPtr ptr(new I3Int(133132));
  ENSURE_EQUAL(ptr.use_count(), 1);
  f.Put("t", ptr);
  ENSURE_EQUAL(ptr.use_count(), 2);
  I3Frame::const_iterator iter = f.begin();

  ENSURE(iter != f.end());
  ENSURE((bool)iter->second);
  I3IntConstPtr newt = boost::dynamic_pointer_cast<const I3Int>(iter->second);
  // hmm, where's #3?
  ENSURE_EQUAL(ptr.use_count(), 4);

  ENSURE_EQUAL(newt->value, 133132);
  ENSURE_EQUAL(iter->second, ptr);
  log_error("ptr @ %p, count %lu", ptr.get(), ptr.use_count());
  //  ENSURE_EQUAL(nslots, 1);
}

TEST(saveload)
{
  I3Frame f;
  I3IntPtr ptr(new I3Int(133132));
  ENSURE_EQUAL(ptr.use_count(), 1);
  f.Put("t", ptr);

  I3FramePtr fp = saveload(f);

  std::cout << "fp: " << *fp;
  ENSURE(fp->has_blob("t"));
  ENSURE(!fp->has_ptr("t"));
  ENSURE(fp->Has("t"));
  I3IntConstPtr ip = fp->Get<I3IntConstPtr>("t");
  ENSURE((bool)ip);
  ENSURE(ip != ptr);
  ENSURE_EQUAL(ip->value, ptr->value);
  ENSURE(fp->has_ptr("t"));
  ENSURE_EQUAL(fp->type_name("t"), icetray::name_of<I3Int>());
}

TEST(a_t_serializes)
{
  I3IntPtr ptr(new I3Int(std::numeric_limits<int>::max()));
  ENSURE((bool)ptr);

  std::ofstream ofs("i3int");
  icecube::archive::portable_binary_oarchive poa(ofs);
  poa << make_nvp("t", ptr);
  remove("i3int");
}
  
TEST(emptyslots)
{
  I3Frame f;
  ENSURE(!f.Get<I3IntConstPtr>("nowhere"));
}

TEST(typename_iterator)
{
  I3Frame f;
  f.Put("int0", I3IntPtr(new I3Int(1)));
  f.Put("int1", I3IntPtr(new I3Int(1)));

  // I3Frame is a hash map, so there is no defined order
  // in which these elements will be retrieved.
  I3Frame::typename_iterator iter = f.typename_begin();
  ENSURE((iter->first=="int0") || (iter->first=="int1"));
  ENSURE_EQUAL(iter->second, icetray::name_of<I3Int>());
  bool first_is_int0 = (iter->first=="int0");

  iter++;
  if (first_is_int0) {
    ENSURE_EQUAL(iter->first, "int1");
  } else {
    ENSURE_EQUAL(iter->first, "int0");
  }
  ENSURE_EQUAL(iter->second, icetray::name_of<I3Int>());

  iter++;
  ENSURE(iter == f.typename_end());
  
  //
  //  verify that typename iterating doesn't deserialize
  // 
  I3FramePtr p = saveload(f);
  ENSURE(p->has_blob("int0") == true);
  ENSURE(p->has_blob("int1") == true);
  ENSURE(p->has_ptr("int0") == false);
  ENSURE(p->has_ptr("int1") == false);

  iter = p->typename_begin();
  while(iter != p->typename_end())
    {
      std::string y = iter->first;
      y = iter->second;
      iter++;
    }

  //
  //  Frame is same as before
  //
  ENSURE(p->has_blob("int0") == true);
  ENSURE(p->has_blob("int1") == true);
  ENSURE(p->has_ptr("int0") == false);
  ENSURE(p->has_ptr("int1") == false);
}

TEST(deserialize_iterator)
{
  I3Frame f;
  f.Put("int0", I3IntPtr(new I3Int(0)));
  ENSURE(f.has_blob("int0") == false);
  f.Put("int1", I3IntPtr(new I3Int(1)));
  ENSURE(f.has_blob("int1") == false);

  I3FramePtr p = saveload(f);

  ENSURE(p->has_ptr("int0") == false);
  ENSURE(p->has_blob("int0") == true);
  ENSURE(p->has_ptr("int1") == false);
  ENSURE(p->has_blob("int1") == true);

  //
  //  if you don't access the object, you don't get deserialize
  //  anyhing.
  //
  I3Frame::const_iterator iter = p->begin();
  while(iter != p->end())
    {
      iter++;
    }
  ENSURE(p->has_ptr("int0") == false);
  ENSURE(p->has_blob("int0") == true);
  ENSURE(p->has_ptr("int1") == false);
  ENSURE(p->has_blob("int1") == true);

  //
  //  if you do look at the pointer, the thing gets deserialized.
  //
  I3Frame::const_iterator iter2 = p->begin();
  while(iter2 != p->end())
    {
      ENSURE((bool)iter2->second);
      iter2++;
    }

  ENSURE(p->has_ptr("int0") == true);
  ENSURE(p->has_blob("int0") == true);
  ENSURE(p->has_ptr("int1") == true);
  ENSURE(p->has_blob("int1") == true);

  //
  //  blob-dropping
  //
  p->drop_blobs(true);
  p->Get<I3FrameObjectConstPtr>("int0");
  ENSURE(p->has_blob("int0") == false);
  p->Get<I3FrameObjectConstPtr>("int1");
  ENSURE(p->has_blob("int1") == false);

}

TEST(take)
{
  I3Frame lhs, rhs;

  rhs.Put("int0", I3IntPtr(new I3Int(0)));

  lhs.take(rhs, "int0");
  
  ENSURE(lhs.has_ptr("int0"));
  ENSURE(lhs.has_blob("int0") == false);

  I3FramePtr p = saveload(rhs);
  
  lhs.take(*p, "int0");
  ENSURE(lhs.has_blob("int0") == true);
  ENSURE(lhs.has_ptr("int0") == false);

}

TEST(rename_w_ptr)
{
  I3Frame f;
  ENSURE(f.Has("i") == false);
  ENSURE(f.Has("j") == false);
  f.Put("i", I3IntPtr());
  ENSURE(f.Has("i") == true);
  ENSURE(f.Has("j") == false);
  f.Rename("i", "j");
  ENSURE(f.Has("i") == false);
  ENSURE(f.Has("j") == true);
}

TEST(rename_w_blob)
{
  I3Frame f;
  f.Put("i", I3IntPtr());
  I3FramePtr p = saveload(f);

  ENSURE(p->Has("i") == true);
  ENSURE(p->has_ptr("i") == false);
  ENSURE(p->has_blob("i") == true);
  
  ENSURE(p->Has("j") == false);
  ENSURE(p->has_ptr("j") == false);
  ENSURE(p->has_blob("j") == false);

  p->Rename("i", "j");

  ENSURE(p->Has("i") == false);
  ENSURE(p->has_ptr("i") == false);
  ENSURE(p->has_blob("i") == false);

  ENSURE(p->Has("j") == true);
  ENSURE(p->has_ptr("j") == false);
  ENSURE(p->has_blob("j") == true);
}

TEST(size)
{
  I3Frame f;
  ENSURE_EQUAL(f.size(), 0u);
  f.Put("one", I3IntPtr());
  ENSURE_EQUAL(f.size(), 1u);
  f.Put("two", I3IntPtr());
  ENSURE_EQUAL(f.size(), 2u);
  f.Rename("two", "TWO");
  ENSURE_EQUAL(f.size(), 2u);
  f.Delete("TWO");
  ENSURE_EQUAL(f.size(), 1u);
  f.Delete("one");
  ENSURE_EQUAL(f.size(), 0u);
}

TEST(find)
{
  I3Frame f;
  f.Put("66", I3IntPtr(new I3Int(66)));
  I3FramePtr p = saveload(f);

  ENSURE(p->has_ptr("66") == false);
  ENSURE(p->has_blob("66") == true);

  I3Frame::const_iterator iter = p->find("66");

  ENSURE(p->has_ptr("66") == false);
  ENSURE(p->has_blob("66") == true);
  I3IntConstPtr pi = boost::dynamic_pointer_cast<const I3Int>(iter->second);
  ENSURE((bool)pi);
  std::cout << "66 should be... " << pi->value << "\n";
  ENSURE_EQUAL(pi->value, 66);

  ENSURE(p->has_ptr("66") == true);
  ENSURE(p->has_blob("66") == true);
}

TEST(saving_creates_bufs)
{
  I3Frame f;
  f.drop_blobs(false);

  f.Put("66", I3IntPtr(new I3Int(66)));
  ENSURE(!f.has_blob("66"));

  ofstream ofs("/dev/null");
  f.save(ofs);
  ENSURE(f.has_blob("66"));
}

TEST(saving_drops_blobs)
{
  I3Frame f;
  f.drop_blobs(true);
  f.Put("66", I3IntPtr(new I3Int(66)));
  ENSURE(!f.has_blob("66"));

  ofstream ofs("/dev/null");
  f.save(ofs);
  ENSURE(!f.has_blob("66"));
}

TEST(duplicated_pointer_fatals_when_saving)
{
  I3IntPtr i(new I3Int(1));
  I3Frame f;
  f.Put("one", i);
  f.Put("one'", i);
  ofstream ofs("/dev/null");
  try {
    f.save(ofs);
    FAIL("save of frame with duplicated pointers didn't throw");
  } catch (...) {
    // ok
  }
}

TEST(ok_to_save_multiple_nulls)
{
  I3Frame f;
  f.Put("one", I3FrameObjectPtr());
  f.Put("other", I3FrameObjectPtr());
  ofstream ofs("/dev/null");
  I3FramePtr f2 = saveload(f);

  ENSURE(f.Has("one"));
  ENSURE(f.Has("other"));
  ENSURE(!f.Get<I3FrameObjectConstPtr>("one"));
  ENSURE(!f.Get<I3FrameObjectConstPtr>("other"));
  // check real quick
  try {
    f.Get<I3FrameObjectConstPtr>("elsewhere");
    FAIL("difference between getting an existent null and something nonexistent..");
  } catch (...) {
  }
}

TEST(typename_survives_serialization)
{
  I3Frame f;
  f.Put("int", I3IntPtr(new I3Int(16)));

  ENSURE_EQUAL(f.type_name("int"), icetray::name_of<I3Int>());

  I3FramePtr fp = saveload(f);

  ENSURE(fp->Has("int"));
  ENSURE(!fp->has_ptr("int"));
  ENSURE_EQUAL(fp->type_name("int"), icetray::name_of<I3Int>());
}

TEST(oserialization_has_shared_semantics)
{
  I3Frame f, g;
  f.drop_blobs(false);
  g.drop_blobs(false);

  I3IntPtr i(new I3Int(661));

  f.Put("i", i);
  ENSURE_EQUAL(i.use_count(), 2);

  g.take(f, "i");
  // f and g now share a struct that contains i, but not i itself.
  ENSURE_EQUAL(i.use_count(), 2);

  ENSURE(f.has_blob("i") == false);
  ENSURE(g.has_blob("i") == false);

  ENSURE(f.has_ptr("i") == true);
  ENSURE(g.has_ptr("i") == true);

  // so they've both got ptrs but no bufs.  Now if we save one, the
  // other should get a buf "for free"

  saveload(f); // this returned a new frame which we let go out of scope

  ENSURE(f.has_ptr("i"));
  ENSURE(g.has_ptr("i"));
  ENSURE(f.has_blob("i"));
  ENSURE(g.has_blob("i"));
}

TEST(delete_has_shared_semantics)
{
  I3Frame f, g;
  I3IntPtr i(new I3Int(661));

  f.Put("i", i);
  ENSURE_EQUAL(i.use_count(), 2);
  ENSURE_EQUAL(f.Get<I3Int>("i").value, 661);

  g.take(f, "i");
  // f and g now share a struct that contains i, but not i itself.
  ENSURE_EQUAL(i.use_count(), 2);
  ENSURE_EQUAL(g.Get<I3Int>("i").value, 661);

  f.Delete("i");
  ENSURE(!f.Has("i"));
  ENSURE_EQUAL(i.use_count(), 2);
  ENSURE_EQUAL(g.Get<I3Int>("i").value, 661);
}

TEST(type_id)
{
  I3Frame f;
  I3IntPtr i(new I3Int(661));
  f.Put("i", i);

  const std::type_info* ti = f.type_id("i");
  log_trace("mine = %s vs %s", I3::name_of(typeid(i.get())).c_str(), I3::name_of(*ti).c_str());
  ENSURE_EQUAL(ti, &typeid(*i.get()));
}

TEST(merge_doesnt_mutate_stream)
{
  I3Frame f('a'), g('b');
  f.merge(g);
  ENSURE_EQUAL(f.GetStop().id(), 'a');
}

TEST(assign_mutates_stream)
{
  I3Frame f(I3Frame::Geometry), g(I3Frame::Calibration);
  f = g;
  ENSURE_EQUAL(f.GetStop(), I3Frame::Calibration);
}

TEST(merge)
{
  I3Frame f, g;
  I3IntPtr i(new I3Int(661)), j(new I3Int(662));
  f.Put("i", i);
  g.Put("j", j);
  ENSURE(f.Has("i"));
  ENSURE(!f.Has("j"));
  ENSURE(!g.Has("i"));
  ENSURE(g.Has("j"));
  g.merge(f);
  ENSURE(g.Has("i"));
  ENSURE(g.Has("j"));
}


TEST(assign)
{
  I3Frame f, g;
  I3IntPtr i(new I3Int(661)), j(new I3Int(662));
  f.Put("i", i);
  g.Put("j", j);
  ENSURE(f.Has("i"));
  ENSURE(!f.Has("j"));
  ENSURE(!g.Has("i"));
  ENSURE(g.Has("j"));
  ENSURE_EQUAL(f.size(), 1u); 
  ENSURE_EQUAL(g.size(), 1u); 

  g = f;

  ENSURE(f.Has("i"));
  ENSURE(!f.Has("j"));
  ENSURE(g.Has("i"));
  ENSURE(!g.Has("j"));
  ENSURE_EQUAL(f.size(), 1u); 
  ENSURE_EQUAL(g.size(), 1u); 
}

TEST(putemptystringframeobjname)
{ 
  I3Frame f;
  I3IntPtr i(new I3Int(661));
  try {
    f.Put("", i);
    FAIL("Should have failed to add a empty string named item");
  } catch (const std::exception& e) { }
}

TEST(putspacefilledstringframeobjname)
{
  I3Frame f;
  I3IntPtr i(new I3Int(661));
  try {
    f.Put("  ", i);
    FAIL("Should have failed to add a space only string named item");
  } catch (const std::exception& e) { }
}

