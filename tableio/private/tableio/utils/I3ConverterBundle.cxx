/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$
 */

#include <icetray/I3FrameObject.h>
#include <tableio/utils/I3ConverterBundle.h>

//***************************************************************************//

I3ConverterBundle::I3ConverterBundle(std::vector<I3ConverterPtr>& converters) 
    : I3Converter(), converters_(converters) {};

//***************************************************************************//

size_t I3ConverterBundle::GetNumberOfRows(const I3FrameObject& object) {
    log_fatal("I should never be called!");
};


//***************************************************************************//

size_t I3ConverterBundle::GetNumberOfRows(I3FrameObjectConstPtr object) {
    std::vector<I3ConverterPtr>::iterator it;
    size_t global_numrows = 0;
    size_t numrows = 0;
    
    for (it = converters_.begin(); it != converters_.end(); it++) {
        numrows = (*it)->GetNumberOfRows(object);
        if (it != converters_.begin() && (numrows != global_numrows)) {
            log_fatal("Two converters returned a different number of rows for the same frame object.");
        }
        global_numrows = numrows;
    }
    
    return global_numrows;
};




//***************************************************************************//

I3TableRowDescriptionConstPtr I3ConverterBundle::GetDescription(I3FrameObjectConstPtr object) {
    return GetDescription(*object);
};

//***************************************************************************//

I3TableRowDescriptionConstPtr I3ConverterBundle::GetDescription(const I3FrameObject& object) {
    std::vector<I3ConverterPtr>::iterator it;

    // start with a blank description
    I3TableRowDescriptionPtr combinedDescription(new I3TableRowDescription); 
    I3TableRowDescriptionConstPtr description;

    for (it = converters_.begin(); it != converters_.end(); it++) {
        description = (*it)->GetDescription(object);
        // concatenate the descriptions
        *combinedDescription << *description;
    }

    return combinedDescription;
};

//***************************************************************************//

size_t I3ConverterBundle::Convert(I3FrameObjectConstPtr object, 
                                          I3TableRowPtr rows, 
                                          I3FramePtr frame) {
    return Convert(*object, rows, frame);
}

//***************************************************************************//

size_t I3ConverterBundle::Convert(const I3FrameObject& object, 
                                          I3TableRowPtr rows, 
                                          I3FramePtr frame) {
    std::vector<I3ConverterPtr>::iterator it;
    size_t global_numrows = 0;
    size_t numrows = 0;
    
    for (it = converters_.begin(); it != converters_.end(); it++) {
       numrows = (*it)->Convert(object,rows,frame);
       if (numrows > global_numrows) global_numrows = numrows;
    }
    
    return global_numrows;
}

//***************************************************************************//

// By convention, only asks the _first_ converter
I3Converter::ConvertState
I3ConverterBundle::CanConvert(I3FrameObjectConstPtr object) {
    return converters_.front()->CanConvert(object);
};

I3Frame::Stream I3ConverterBundle::GetStop(){
  std::vector<I3ConverterPtr>::iterator it = converters_.begin();
  I3Frame::Stream stream = (*it)->GetStop();
  it++;
  for (; it != converters_.end(); it++) {
    if ((*it)->GetStop() != stream){
      log_fatal("This ConverterBundle has converters with different frame streams");
    }
  }
  return stream;
}
