/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_xml_archive.cpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#define I3_ARCHIVE_SOURCE
#include <archive/basic_xml_archive.hpp>

namespace icecube {
namespace archive {

I3_ARCHIVE_DECL(const char *)
I3_ARCHIVE_XML_OBJECT_ID(){
    return "object_id";
}
I3_ARCHIVE_DECL(const char *)
I3_ARCHIVE_XML_OBJECT_REFERENCE(){
    return "object_id_reference";
}
I3_ARCHIVE_DECL(const char *)
I3_ARCHIVE_XML_CLASS_ID(){
    return "class_id";
}
I3_ARCHIVE_DECL(const char *)
I3_ARCHIVE_XML_CLASS_ID_REFERENCE(){
    return "class_id_reference";
}
I3_ARCHIVE_DECL(const char *)
I3_ARCHIVE_XML_CLASS_NAME(){
    return "class_name";
}
I3_ARCHIVE_DECL(const char *)
I3_ARCHIVE_XML_TRACKING(){
    return "tracking_level";
}
I3_ARCHIVE_DECL(const char *)
I3_ARCHIVE_XML_VERSION(){
    return "version";
}
I3_ARCHIVE_DECL(const char *)
I3_ARCHIVE_XML_SIGNATURE(){
    return "signature";
}

}// namespace archive
}// namespace icecube
