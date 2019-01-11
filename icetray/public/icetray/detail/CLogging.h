#ifndef ICETRAY_DETAIL_CLOGGING_H_INCLUDED
#define ICETRAY_DETAIL_CLOGGING_H_INCLUDED

//
// c-linkage logging functions.  
//

#ifdef __cplusplus
extern "C" 
{
#endif
  void i3_clog_trace(const char* logger, const char* msg);
  void i3_clog_debug(const char* logger, const char* msg);
  void i3_clog_info(const char* logger, const char* msg);
  void i3_clog_warn(const char* logger, const char* msg);
  void i3_clog_error(const char* logger, const char* msg);
  void i3_clog_fatal(const char* logger, const char* msg);
#ifdef __cplusplus
}
#endif
#endif
