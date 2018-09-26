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

#include "tableio/detail/I3ConverterMill.h"
#include <boost/python/extract.hpp>

/// @cond
I3ConverterMill::I3ConverterMill(boost::python::object callable)
    : callable_(callable)
{
	thneed_ = (*this)();
	if (!thneed_)
		throw std::runtime_error("Couldn't instantiate converter!");
}
	
bool
I3ConverterMill::CanConvert(I3FrameObjectConstPtr object)
{
	return thneed_->CanConvert(object);
}
	
I3ConverterPtr
I3ConverterMill::operator()()
{
	return boost::python::extract<I3ConverterPtr>(callable_());
}
/// @endcond
