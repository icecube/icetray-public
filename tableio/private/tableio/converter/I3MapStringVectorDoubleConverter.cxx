#include "tableio/converter/I3MapStringVectorDoubleConverter.h"
#include "tableio/I3Converter.h"
#include "boost/foreach.hpp"
#include <tableio/converter/container_converter_detail.h>

size_t I3MapStringVectorDoubleConverter::GetNumberOfRows(const I3MapStringVectorDouble& map) {
        I3MapStringVectorDouble::const_iterator mapiter = map.begin();
        size_t maxindex = mapiter->second.size();

        return maxindex;
    }

I3TableRowDescriptionPtr I3MapStringVectorDoubleConverter::CreateDescription(const I3MapStringVectorDouble& m) {
    I3TableRowDescriptionPtr desc(new I3TableRowDescription());
        desc->isMultiRow_ = true;
        desc->AddField<tableio_size_t>("vector_index", "", "index in vector");
    BOOST_FOREACH( I3MapStringVectorDouble::value_type p, m) {
        desc->AddField<double>(p.first,"","");
    }
    if (m.size() == 0)
      log_error("Warning:  I3MapStringVectorDouble used for converter configuration is EMPTY, nothing will be booked");
    return desc;
};

size_t I3MapStringVectorDoubleConverter::FillRows(const I3MapStringVectorDouble& map, I3TableRowPtr rows) {
    I3MapStringVectorDouble::const_iterator mapiter = map.begin();
    size_t maxindex = mapiter->second.size();

    for(mapiter = map.begin();
        mapiter != map.end();
        mapiter++)
    { 
        for (size_t i = 0; i < mapiter->second.size(); i++)
        {
            rows->SetCurrentRow(i);
            rows->Set<double>(mapiter->first,mapiter->second[i]);
        }
    }

    for(size_t i = 0; i < maxindex; i++)
    {
        rows->SetCurrentRow(i);
        rows->Set<size_t>("vector_index",i);
    }

    return maxindex;
};
