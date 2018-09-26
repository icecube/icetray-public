/*
 * copyright  (C) 2012
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy$
 */

#ifndef TABLEIO_DETAIL_I3CONVERTERMILL_H_INCLUDED
#define TABLEIO_DETAIL_I3CONVERTERMILL_H_INCLUDED

#include "tableio/I3Converter.h"
#include <boost/python/object.hpp>

/// @cond 
class I3ConverterMill {
public:
	I3ConverterMill(boost::python::object);
	bool CanConvert(I3FrameObjectConstPtr);
	I3ConverterPtr operator()();
private:
	boost::python::object callable_;
	I3ConverterPtr thneed_;
};

I3_POINTER_TYPEDEFS(I3ConverterMill);

/// @endcond

#endif // TABLEIO_DETAIL_I3CONVERTERMILL_H_INCLUDED