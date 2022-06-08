//
//   Copyright (c) 2015
//   the IceCube Collaboration
//
//   $Id: $
//
//   @file TriggerKey.cxx
//   @version
//   @date $Date: 2015-12-10$
//   @author mjurkovic <martin.jurkovic@tum.de>
//
//   This file is part of IceTray.
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//   
//   THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
//   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
//   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
//   OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
//   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
//   SUCH DAMAGE.
//   
//   SPDX-License-Identifier: BSD-2-Clause
//   
//


#include <dataclasses/TriggerKey.h>
#include <icetray/python/dataclass_suite.hpp>
#include <dataclasses/ostream_overloads.hpp>

using namespace boost::python;

using boost::python::object;
object
get_config_id_optional( const TriggerKey& tk) {
    if( tk.GetConfigIDOptional() ) {
        return object(tk.GetConfigIDOptional().get() );
    }
    else {
        return object();
    }
}

namespace pyTriggerKey {
    // Get source from string
    TriggerKey::SourceID GetSourceFromString(TriggerKey &object,
            const std::string& sourcestr)
        {return object.GetSourceFromString(sourcestr);};

    // Get source string
    const char* GetSourceString(TriggerKey &object, TriggerKey::SourceID source)
        {return object.GetSourceString(source);};

    // Get type from string
    TriggerKey::TypeID GetTypeFromString(TriggerKey &object,
            const std::string& typestr)
        {return object.GetTypeFromString(typestr);};

    // Get type string
    const char* GetTypeString(TriggerKey &object, TriggerKey::TypeID type)
        {return object.GetTypeString(type);};
}

#define ENUM_DEF(r,data,T) .value(BOOST_PP_STRINGIZE(T), data::T)

std::string repr(const TriggerKey& key){
  std::stringstream s;
  s << "TriggerKey" << key;
  return s.str();
}

void
register_TriggerKey()
{

    class_<TriggerKey>("TriggerKey")
        .def(init<TriggerKey::SourceID, TriggerKey::TypeID>() )
        .def(init<TriggerKey::SourceID, TriggerKey::TypeID, int>() )
        #define PROPERTIES (Source)(Type)(Subtype)
        BOOST_PP_SEQ_FOR_EACH(WRAP_PROP, TriggerKey, PROPERTIES)
        #undef  PROPERTIES
        .add_property("config_id", get_config_id_optional,
                (void (TriggerKey::*)(int)) &TriggerKey::SetConfigID)
        .def("check_config_id", &TriggerKey::CheckConfigID)
        .def("reset_config_id",
                (void (TriggerKey::*)()) &TriggerKey::SetConfigID)
        .def("__str__", &stream_to_string<TriggerKey>)
        .def("__repr__", repr)
        .def("get_source_from_string",
                &pyTriggerKey::GetSourceFromString)
        .def("get_source_string", &pyTriggerKey::GetSourceString)
        .def("get_type_from_string", &pyTriggerKey::GetTypeFromString)
        .def("get_type_string", &pyTriggerKey::GetTypeString)
        .def(self < self)
        .def(self >= self)
        .def(self > self)
        .def(self <= self)
        .def(self == self)
        .def(self != self)
        .def(dataclass_suite<TriggerKey>())
        ;

    enum_<TriggerKey::SourceID>("SourceID")
        BOOST_PP_SEQ_FOR_EACH(ENUM_DEF, TriggerKey,
                TRIGGERKEY_H_TriggerKey_SourceID)
        .export_values()
        ;

    enum_<TriggerKey::TypeID>("TypeID")
        BOOST_PP_SEQ_FOR_EACH(ENUM_DEF, TriggerKey,
                TRIGGERKEY_H_TriggerKey_TypeID)
        .export_values()
        ;

    enum_<TriggerKey::SubtypeID>("SubtypeID")
        BOOST_PP_SEQ_FOR_EACH(ENUM_DEF, TriggerKey,
                TRIGGERKEY_H_TriggerKey_SubtypeID)
        .export_values()
        ;

    def("identity", identity_<TriggerKey::SourceID>);
    def("identity", identity_<TriggerKey::TypeID>);
    def("identity", identity_<TriggerKey::SubtypeID>);



}

