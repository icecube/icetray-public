#include <stdint.h>

#include <iostream>
#include <cmath>
#include <vector>
#include <map>
#include <string>
#include <fstream>

#include <icetray/serialization.h>
#include <icetray/I3Logging.h>
#include <icetray/IcetrayFwd.h>
#include <icetray/I3DefaultName.h>
#include <icetray/I3FrameObject.h>
#include <icetray/I3PointerTypedefs.h>
#include <icetray/I3Configuration.h>
#include <icetray/I3Module.h>
#include <icetray/I3ServiceFactory.h>
#include <icetray/I3Factory.h>
#include <icetray/OMKey.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Context.h>

#include <I3/name_of.h>

#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/add_reference.hpp>
#include <boost/type_traits/add_const.hpp>
#include <boost/static_assert.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/preprocessor.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/regex.hpp>



