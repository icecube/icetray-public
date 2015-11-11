#include <I3Test.h>
#include <icetray/I3FrameMixing.h>

#include <icetray/I3Int.h>
#include <boost/make_shared.hpp>

TEST_GROUP(I3FrameMixing);

TEST(FirstFrameUnaltered){
	I3Frame f(I3Frame::DAQ);
	f.Put("i1",boost::make_shared<I3Int>(1));
	I3FrameMixer mixer;
	mixer.Mix(f);
	
	ENSURE(f.Has("i1"),"Frame should retain original keys");
	ENSURE_EQUAL(f.keys().size(),1,"Frame should gain no additional keys");
}

TEST(Immiscible_TrayInfo){
	I3Frame f1(I3Frame::TrayInfo);
	f1.Put("i1",boost::make_shared<I3Int>(1));
	
	I3Frame f2(I3Frame::DAQ);
	
	I3FrameMixer mixer;
	mixer.Mix(f1);
	mixer.Mix(f2);
	
	ENSURE_EQUAL(f2.keys().size(),0,"I frame keys should not mix into other streams");
}

TEST(Immiscible_Physics){
	I3Frame f1(I3Frame::Physics);
	f1.Put("i1",boost::make_shared<I3Int>(1));
	
	I3Frame f2(I3Frame::DAQ);
	
	I3FrameMixer mixer;
	mixer.Mix(f1);
	mixer.Mix(f2);
	
	ENSURE_EQUAL(f2.keys().size(),0,"P frame keys should not mix into other streams");
}

TEST(MixQToP){
	I3Frame f1(I3Frame::DAQ);
	f1.Put("i1",boost::make_shared<I3Int>(1));
	
	I3Frame f2(I3Frame::Physics);
	
	I3FrameMixer mixer;
	mixer.Mix(f1);
	mixer.Mix(f2);
	
	ENSURE(f2.Has("i1"),"Q frame keys should mix into P frames");
	ENSURE_EQUAL(f2.keys().size(),1,"Only one key should be added");
}

TEST(MixMany){
	I3Frame f1(I3Frame::Geometry);
	f1.Put("i1",boost::make_shared<I3Int>(1));
	I3Frame f2(I3Frame::Calibration);
	f2.Put("i2",boost::make_shared<I3Int>(2));
	I3Frame f3(I3Frame::DetectorStatus);
	f3.Put("i3",boost::make_shared<I3Int>(3));
	I3Frame f4(I3Frame::DAQ);
	f4.Put("i4",boost::make_shared<I3Int>(4));
	I3Frame f5('!'); //non standard streams should also work!
	f5.Put("i5",boost::make_shared<I3Int>(5));
	
	I3Frame f6(I3Frame::Physics);
	
	I3FrameMixer mixer;
	mixer.Mix(f1);
	mixer.Mix(f2);
	mixer.Mix(f3);
	mixer.Mix(f4);
	mixer.Mix(f5);
	mixer.Mix(f6);
	
	ENSURE(f2.Has("i1"),"Other streams should mix into C frames");
	ENSURE_EQUAL(f2.keys().size(),2,"All previous keys should be added, and no more");
	
	ENSURE(f3.Has("i1"),"Other streams should mix into D frames");
	ENSURE(f3.Has("i2"),"Other streams should mix into D frames");
	ENSURE_EQUAL(f3.keys().size(),3,"All previous keys should be added, and no more");
	
	ENSURE(f4.Has("i1"),"Other streams should mix into Q frames");
	ENSURE(f4.Has("i2"),"Other streams should mix into Q frames");
	ENSURE(f4.Has("i3"),"Other streams should mix into Q frames");
	ENSURE_EQUAL(f4.keys().size(),4,"All previous keys should be added, and no more");
	
	ENSURE(f5.Has("i1"),"Other streams should mix into made-up frames");
	ENSURE(f5.Has("i2"),"Other streams should mix into made-up frames");
	ENSURE(f5.Has("i3"),"Other streams should mix into made-up frames");
	ENSURE(f5.Has("i4"),"Other streams should mix into made-up frames");
	ENSURE_EQUAL(f5.keys().size(),5,"All previous keys should be added, and no more");
	
	ENSURE(f6.Has("i1"),"Other streams should mix into P frames");
	ENSURE(f6.Has("i2"),"Other streams should mix into P frames");
	ENSURE(f6.Has("i3"),"Other streams should mix into P frames");
	ENSURE(f6.Has("i4"),"Other streams should mix into P frames");
	ENSURE(f6.Has("i5"),"Other streams should mix into P frames");
	ENSURE_EQUAL(f6.keys().size(),5,"All previous keys should be added, and no more");
}

TEST(MixingDoesNotOverrideNative){
	I3Frame f1(I3Frame::DAQ);
	f1.Put("i1",boost::make_shared<I3Int>(1));
	
	I3Frame f2(I3Frame::Physics);
	f2.Put("i1",boost::make_shared<I3Int>(15));
	
	I3FrameMixer mixer;
	mixer.Mix(f1);
	mixer.Mix(f2);
	
	ENSURE(f2.Has("i1"),"Key should still be present");
	ENSURE_EQUAL(f2.keys().size(),1,"No other keys should be added");
	ENSURE_EQUAL(f2.Get<I3Int>("i1").value,15,"Mixing should not overwrite native keys");
}

TEST(MostRecentKeysAreMixed){
	I3Frame f1(I3Frame::DAQ);
	f1.Put("i1",boost::make_shared<I3Int>(1));
	I3Frame f2(I3Frame::DAQ);
	f2.Put("i1",boost::make_shared<I3Int>(2));
	
	I3Frame f3(I3Frame::Physics);
	
	I3FrameMixer mixer;
	mixer.Mix(f1);
	mixer.Mix(f2);
	mixer.Mix(f3);
	
	ENSURE(f3.Has("i1"),"Q frame keys should mix into P frames");
	ENSURE_EQUAL(f3.keys().size(),1,"Only one key should be added");
	ENSURE_EQUAL(f3.Get<I3Int>("i1").value,2,"Most recent value from other stream should be present");
}

TEST(OldMixedKeysArePurged){
	I3Frame f1(I3Frame::DAQ);
	f1.Put("i1",boost::make_shared<I3Int>(1));
	I3Frame f2(I3Frame::DAQ);
	f2.Put("i2",boost::make_shared<I3Int>(2));
	
	I3Frame f3(I3Frame::Physics);
	
	I3FrameMixer mixer;
	mixer.Mix(f1);
	mixer.Mix(f2);
	mixer.Mix(f3);
	
	ENSURE(not f3.Has("i1"),"Old mixed keys should be removed");
	ENSURE(f3.Has("i2"),"Q frame keys should mix into P frames");
	ENSURE_EQUAL(f3.keys().size(),1,"Only one key should be added");
}

TEST(MixedFrameRetrieval){
	I3Frame f1(I3Frame::Calibration);
	f1.Put("i1",boost::make_shared<I3Int>(1));
	I3Frame f2(I3Frame::Calibration);
	f2.Put("i2",boost::make_shared<I3Int>(2));
	I3Frame f3(I3Frame::DetectorStatus);
	f3.Put("i3",boost::make_shared<I3Int>(3));
	I3Frame f4(I3Frame::DetectorStatus);
	f4.Put("i4",boost::make_shared<I3Int>(4));
	
	{ //in-order, for another frame type
		I3FrameMixer mixer(true);
		mixer.Mix(f1);
		mixer.Mix(f3);
		mixer.Mix(f4);
		mixer.Mix(f2);
		
		std::vector<boost::shared_ptr<I3Frame> > frames =
		 mixer.GetMixedFrames(I3Frame::Physics);
		
		ENSURE_EQUAL(frames.size(),2);
		ENSURE_EQUAL(frames[0]->GetStop(),I3Frame::DetectorStatus);
		ENSURE_EQUAL(frames[1]->GetStop(),I3Frame::Calibration);
		ENSURE(frames[0]->Has("i4"));
		ENSURE(frames[1]->Has("i2"));
	}
	
	{ //in-order, a cached frame type
		I3FrameMixer mixer(true);
		mixer.Mix(f1);
		mixer.Mix(f3);
		mixer.Mix(f4);
		mixer.Mix(f2);
		
		std::vector<boost::shared_ptr<I3Frame> > frames =
		mixer.GetMixedFrames(I3Frame::Calibration);
		
		ENSURE_EQUAL(frames.size(),1);
		ENSURE_EQUAL(frames[0]->GetStop(),I3Frame::DetectorStatus);
		ENSURE(frames[0]->Has("i4"));
	}
	
	{ //out-of-order, for another frame type
		I3FrameMixer mixer(false);
		mixer.Mix(f1);
		mixer.Mix(f3);
		mixer.Mix(f4);
		mixer.Mix(f2);
		
		std::vector<boost::shared_ptr<I3Frame> > frames =
		mixer.GetMixedFrames(I3Frame::Physics);
		
		ENSURE_EQUAL(frames.size(),2);
		if(frames[0]->GetStop()!=I3Frame::DetectorStatus)
			std::swap(frames[0],frames[1]);
		ENSURE_EQUAL(frames[0]->GetStop(),I3Frame::DetectorStatus);
		ENSURE_EQUAL(frames[1]->GetStop(),I3Frame::Calibration);
		ENSURE(frames[0]->Has("i4"));
		ENSURE(frames[1]->Has("i2"));
	}
	
	{ //out-of-order, a cached frame type
		I3FrameMixer mixer(false);
		mixer.Mix(f1);
		mixer.Mix(f3);
		mixer.Mix(f4);
		mixer.Mix(f2);
		
		std::vector<boost::shared_ptr<I3Frame> > frames =
		mixer.GetMixedFrames(I3Frame::Calibration);
		
		ENSURE_EQUAL(frames.size(),1);
		ENSURE_EQUAL(frames[0]->GetStop(),I3Frame::DetectorStatus);
		ENSURE(frames[0]->Has("i4"));
	}
}
