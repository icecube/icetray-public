#include <icetray/I3Factory.h>
#include <icetray/I3Logging.h>

void emitDeprecationWarning(const char* kind, const char* name)
{
  log_warn("\n**\n"
           "**    You are using the %s '%s'.\n"
           "**    This %s is DEPRECATED, and will soon\n"
           "**    no longer be included in meta-project releases\n"
           "**    Reasons for this include lack of maintainership or a\n"
           "**    new implementation under a different name.  If you\n"
           "**    believe %s should not be removed, please email\n"
           "**    dataclass@icecube.wisc.edu or the appropriate\n"
           "**    meta-project mailing list.  You can also file a \n"
           "**    bug report at http://code.icecube.wisc.edu.\n"
           "**\n",
           kind, name, kind, name);
}

