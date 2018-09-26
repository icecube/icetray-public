
#include <icetray/I3Module.h>
#include <dataclasses/geometry/I3Geometry.h>

class TestConstCache : public I3Module {
public:
  TestConstCache(const I3Context& ctx) : 
    I3Module(ctx),
    test_dom_(21,30)
  { 
    AddOutBox("OutBox"); 
  }

  void Geometry(I3FramePtr frame){
    cached_geo_ = frame->Get<I3GeometryConstPtr>();
    test_value_ = cached_geo_->omgeo.at(test_dom_).omtype;
  }

  void DAQ(I3FramePtr frame){    
    // i've cached a pointer to a const geometry
    // so this should never change.
    if(cached_geo_->omgeo.at(test_dom_).omtype != test_value_){
      log_fatal("The cached const object changed from %d to %d!!!",
                test_value_, cached_geo_->omgeo.at(test_dom_).omtype);
    }
  }

  void Configure(){}

private:

  I3GeometryConstPtr cached_geo_;
  const OMKey test_dom_;
  I3OMGeo::OMType test_value_;
};

I3_MODULE(TestConstCache);
