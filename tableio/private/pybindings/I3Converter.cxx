/**
 * I3Converter.cxx (pybindings)
 *
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy$
 */


#include <tableio/converter/PythonConverter.h>
#include "tableio/detail/I3ConverterMill.h"

namespace bp = boost::python;

void register_I3Converter() {
	bp::enum_<I3Converter::ConvertState>("ConvertState")
	.value("NoConversion", I3Converter::NoConversion)
	.value("ExactConversion", I3Converter::ExactConversion)
	.value("InexactConversion", I3Converter::InexactConversion)
	;
        
	bp::class_<PythonConverter, boost::shared_ptr<PythonConverter>, boost::noncopyable>
		("I3Converter")

	.add_property("description",
		(I3TableRowDescriptionConstPtr (PythonConverter::*)()) &PythonConverter::GetDescription)
	.add_property("current_frame",
		(I3FramePtr (PythonConverter::*)()) &PythonConverter::GetCurrentFrame)
	.def("CreateDescription",(I3TableRowDescriptionPtr (PythonConverter::*)(I3FrameObjectConstPtr))
                    &PythonConverter::CreateDescription)
	.def("Convert",(size_t (PythonConverter::*)(I3FrameObjectConstPtr, 
	                                                  I3TableRowPtr, 
	                                                  I3FramePtr))
	               &PythonConverter::Convert)
	.def("CanConvert", (I3Converter::ConvertState (PythonConverter::*)(I3FrameObjectPtr)) &PythonConverter::CanConvert)
	.def("FillRows",&PythonConverter::FillRows)
	//.def("GetNumberOfRows",&PythonConverter::GetNumberOfRows)
	.def("GetNumberOfRows",(size_t (PythonConverter::*)(I3FrameObjectConstPtr))
                           &PythonConverter::GetNumberOfRows)
          .def("GetStop",
               &PythonConverter::GetStop)

          ; 
	
	bp::class_<I3ConverterMill, I3ConverterMillPtr >
	    ("I3ConverterMill", bp::init<bp::object>())
	;

}

