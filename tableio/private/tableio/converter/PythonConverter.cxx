#include <icetray/I3FrameObject.h>

#include <tableio/converter/PythonConverter.h>
#include <boost/python.hpp>

namespace bp = boost::python;

size_t PythonConverter::GetNumberOfRows(I3FrameObjectConstPtr object) {
	log_trace("%s",__PRETTY_FUNCTION__);
	if (bp::override num_rows = this->get_override("GetNumberOfRows")) {
		return num_rows(boost::const_pointer_cast<I3FrameObject>(object));
	} else {
		return 1;
	}
}

size_t PythonConverter::GetNumberOfRows(const I3FrameObject& object) {
	log_trace("%s",__PRETTY_FUNCTION__);    
    log_fatal("I shouldn't ever be called!");
}


I3TableRowDescriptionConstPtr PythonConverter::GetDescription(I3FrameObjectConstPtr object) {
    // return GetDescription(*object);
	if (description_)
	        return description_;
	    else {
	        description_ = CreateDescription(object); 
	        return description_;
    }
}

I3TableRowDescriptionConstPtr PythonConverter::GetDescription(const I3FrameObject& object) {
	log_trace("%s",__PRETTY_FUNCTION__);    
    log_fatal("I shouldn't ever be called!");

}

I3TableRowDescriptionConstPtr PythonConverter::GetDescription() {
	log_trace("%s",__PRETTY_FUNCTION__);    
	if (description_)
	        return description_;
	    else {
		log_fatal("No description set.");
	    }
}

I3TableRowDescriptionPtr PythonConverter::CreateDescription(const I3FrameObject& object) {
	log_trace("%s",__PRETTY_FUNCTION__);
	if (bp::override create_desc = this->get_override("CreateDescription")) {
		I3TableRowDescriptionPtr desc_ptr = create_desc(object);
		if (!desc_ptr) {
			log_fatal("CreateDescription(frame_object) must return an I3TableRowDescription.");
		} else {
			return desc_ptr;
		}
	} else {
		log_fatal("Python module must implement CreateDescription(frame_object).");
	}
}

I3TableRowDescriptionPtr PythonConverter::CreateDescription(I3FrameObjectConstPtr object) {
	log_trace("%s",__PRETTY_FUNCTION__);
	if (bp::override create_desc = this->get_override("CreateDescription")) {
		I3TableRowDescriptionPtr desc_ptr = create_desc(boost::const_pointer_cast<I3FrameObject>(object));
		if (!desc_ptr) {
			log_fatal("CreateDescription(frame_object) must return an I3TableRowDescription.");
		} else {
			return desc_ptr;
		}
	} else {
		log_fatal("Python module must implement CreateDescription(frame_object).");
	}
}


size_t PythonConverter::Convert(I3FrameObjectConstPtr object, 
                             I3TableRowPtr rows, 
                             I3FramePtr frame) {
	log_trace("%s",__PRETTY_FUNCTION__);
	if (bp::override fillrows = this->get_override("FillRows")) {
        currentFrame_ = frame;
		size_t nrows = fillrows(boost::const_pointer_cast<I3FrameObject>(object),rows);
        currentFrame_.reset();
        return nrows;
	} else if (bp::override convert = this->get_override("Convert")) {
		return convert(boost::const_pointer_cast<I3FrameObject>(object),rows,frame);
	} else {
		log_fatal("Python module must implement Convert(frame_object,rows,frame) or FillRows(frame_object,rows).");
	}
}

size_t PythonConverter::FillRows(const I3FrameObjectConstPtr object, I3TableRowPtr rows) {
	log_trace("%s",__PRETTY_FUNCTION__);
	if (bp::override fillrows = this->get_override("FillRows")) {
		return fillrows(boost::const_pointer_cast<I3FrameObject>(object),rows);
	} else {
		log_fatal("Python module must implement Convert(frame_object,rows,frame) or FillRows(frame_object,rows).");
	}
}


size_t PythonConverter::Convert(const I3FrameObject& object, 
                             I3TableRowPtr rows, 
                             I3FramePtr frame) {
	log_trace("%s",__PRETTY_FUNCTION__);
	if (bp::override convert = this->get_override("Convert")) {
		return convert(object,rows,frame);
	} else {
		log_fatal("Python module must implement Convert(frame_object,rows,frame).");
	}
}

I3Converter::ConvertState
PythonConverter::CanConvert(I3FrameObjectConstPtr object) {
	return CanConvert(boost::const_pointer_cast<I3FrameObject>(object));
}

I3Converter::ConvertState PythonConverter::CanConvert(I3FrameObjectPtr obj) {
	if (bp::override can_convert = this->get_override("CanConvert")) {
		return can_convert(obj);
	} else {
		bp::reference_existing_object::apply<PythonConverter*>::type converter;
		PyObject* sptr = converter( this );
		bp::object object(obj);
		if (!PyObject_HasAttrString(sptr,"booked")) {
			log_fatal("Python module must define attribute 'booked' holding the type to be converted.");
		}
        PyObject* oType=PyObject_Type(object.ptr());
        PyObject* bType=PyObject_GetAttrString(sptr, "booked");
        bool exactMatch=oType==bType;
        bool inexactMatch=PyObject_IsInstance(object.ptr(),bType);
        Py_DECREF(oType);
        Py_DECREF(bType);
        if (exactMatch) {
            return ExactConversion;
        }
        if (inexactMatch) {
            return InexactConversion;
        }
        return NoConversion;
	}
}


I3FramePtr PythonConverter::GetCurrentFrame() {
	log_trace("%s",__PRETTY_FUNCTION__);    
	if (currentFrame_)
	        return currentFrame_;
	else {
		log_fatal("currentFrame_ is not set");
	}
}

I3Frame::Stream PythonConverter::GetStop(){
  if (bp::override get_stream = this->get_override("GetStop")){
    return get_stream();
  }
  return I3Frame::Physics;
}
