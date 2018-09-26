#ifndef TABLEIO_I3MAPSTRINGVECTORDOUBLECONVERTER_H_INCLUDED
#define TABLEIO_I3MAPSTRINGVECTORDOUBLECONVERTER_H_INCLUDED

#include "tableio/I3Converter.h"
#include "dataclasses/I3Map.h"

class I3MapStringVectorDoubleConverter : public I3ConverterImplementation<I3MapStringVectorDouble> {
private:
    I3TableRowDescriptionPtr CreateDescription(const I3MapStringVectorDouble& map);
    size_t FillRows(const I3MapStringVectorDouble& map, I3TableRowPtr rows);
    size_t GetNumberOfRows(const I3MapStringVectorDouble& map);
};

#endif // TABLEIO_I3MAPSTRINGVECTORDOUBLECONVERTER_H_INCLUDED
