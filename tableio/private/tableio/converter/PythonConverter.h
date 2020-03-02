#ifndef TABLEIO_PYTHONCONVERTER_H_INCLUDED
#define TABLEIO_PYTHONCONVERTER_H_INCLUDED

#include <tableio/I3Converter.h>
#include <boost/python.hpp>
#include <icetray/I3FrameObject.h>
#include <icetray/I3Frame.h>

struct PythonConverter : I3Converter, boost::python::wrapper<I3Converter>
{

    PythonConverter() {};

    // void Configure( const StringPairVector& params);

    size_t GetNumberOfRows(I3FrameObjectConstPtr object);
    size_t GetNumberOfRows(const I3FrameObject& object);

    I3TableRowDescriptionConstPtr GetDescription(I3FrameObjectConstPtr object);
    I3TableRowDescriptionConstPtr GetDescription(const I3FrameObject& object);

    I3TableRowDescriptionConstPtr GetDescription();

    I3TableRowDescriptionPtr CreateDescription(I3FrameObjectConstPtr object);
    I3TableRowDescriptionPtr CreateDescription(const I3FrameObject& object);
    
    size_t Convert(I3FrameObjectConstPtr object, 
                                 I3TableRowPtr rows, 
                                 I3FramePtr frame=I3FramePtr());
    
    size_t Convert(const I3FrameObject& object, 
                                 I3TableRowPtr rows, 
                                 I3FramePtr frame=I3FramePtr());

    size_t FillRows(const I3FrameObjectConstPtr object, I3TableRowPtr rows);

    ConvertState CanConvert(I3FrameObjectConstPtr object);
    ConvertState CanConvert(I3FrameObjectPtr object);

    I3FramePtr GetCurrentFrame();

    I3Frame::Stream GetStop();

    SET_LOGGER("PythonConverter");
};

#endif // TABLEIO_PYTHONCONVERTER_H_INCLUDED
