#ifndef DATACLASSES_I3STRING_H_INCLUDED
#define DATACLASSES_I3STRING_H_INCLUDED

#include <string>

#include <icetray/I3DefaultName.h>
#include <icetray/I3PODHolder.h>

typedef I3PODHolder<std::string> I3String;

I3_POINTER_TYPEDEFS(I3String);
I3_DEFAULT_NAME(I3String);

std::ostream& operator<<(std::ostream& oss, const I3String& s);
bool operator==(const I3String& lhs, std::string rhs);
bool operator!=(const I3String& lhs, std::string rhs);

#endif

