#ifndef ICETRAY_IMPL_H_INCLUDED
#define ICETRAY_IMPL_H_INCLUDED

#include <icetray/I3Logging.h>

#if defined(I3_FRAMEWORK_LOGGING)
#define i3_log(format, ...) log_trace("%s: " format, GetName().c_str(), ##__VA_ARGS__)
#else
#define i3_log(format, ...) 
#endif

#endif
