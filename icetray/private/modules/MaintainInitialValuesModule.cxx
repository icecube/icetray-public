/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @version $Revision$
 * @date $Date$
 * @author troy d. straszheim
 *
 * This tests that the global GetService<> works; that the underlying
 * context-switching is done correctly.
 */

#include <icetray/I3Tray.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Module.h>
#include <icetray/OMKey.h>
#include <boost/assign/std/vector.hpp>

using namespace boost::assign;

static bool bool_param;
static unsigned char uchar_param;
static int int_param;
static long long_param;
static double double_param;
static std::string string_param;
static OMKey omkey_param;

struct MyService{};

struct MaintainInitialValuesModule : I3Module
{
  boost::shared_ptr<MyService> service_ptr_param;

  MaintainInitialValuesModule(const I3Context& context) : I3Module(context) 
  { 

    bool_param = true;
    AddParameter("bool_param", "description of bool", bool_param);

    uchar_param = std::numeric_limits<unsigned char>::max();
    AddParameter("uchar_param", "description of uchar", uchar_param);

    int_param = std::numeric_limits<int>::max();
    AddParameter("int_param", "description of int", int_param);

    long_param = std::numeric_limits<long>::max();
    AddParameter("long_param", "description of long", long_param);

    double_param = 3.1415926535897932;
    AddParameter("double_param", "description of double", double_param);

    string_param = "We can't stop here.  This is Bat Country!";
    AddParameter("string_param", "description of string", string_param);
    
    omkey_param = OMKey(-666,666);
    AddParameter("omkey_param", "OMKey!", omkey_param);

    service_ptr_param = boost::shared_ptr<MyService>( new MyService);
    AddParameter("service_ptr_param", "pointer to service.",service_ptr_param);
  }

  virtual void Configure() 
  { 
    GetParameter("bool_param", bool_param);
    GetParameter("uchar_param", uchar_param);
    GetParameter("int_param", int_param);
    GetParameter("long_param", long_param);
    GetParameter("double_param", double_param);
    GetParameter("string_param", string_param);
    GetParameter("omkey_param", omkey_param);
    GetParameter("service_ptr_param",service_ptr_param);

    i3_assert( bool_param == true);
    i3_assert( uchar_param == std::numeric_limits<unsigned char>::max() );
    i3_assert( int_param == std::numeric_limits<int>::max());
    i3_assert( long_param == std::numeric_limits<long>::max());
    i3_assert( double_param == 3.1415926535897932);
    i3_assert( string_param == "We can't stop here.  This is Bat Country!");
    i3_assert( omkey_param == OMKey(-666,666));
    i3_assert( service_ptr_param ); 
  }

  virtual void Process() { 
    I3FramePtr frame = PopFrame(); 
    PushFrame(frame, "OutBox"); 
  }

  virtual void Finish() { ; }
};

I3_MODULE(MaintainInitialValuesModule);
