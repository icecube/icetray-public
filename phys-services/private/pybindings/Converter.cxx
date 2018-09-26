/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Fabian Kislat <fabian.kislat@desy.de> $LastChangedBy$
 */

#include <tableio/converter/pybindings.h>
#include "../converter/I3EventInfoConverter.h"
#include "../converter/I3RecoInfoConverter.h"
#include "../converter/I3CutValuesConverter.h"
#include "phys-services/I3ScaleCalculator.h"



void register_Converter() {
    I3CONVERTER_NAMESPACE(phys_services);

    //typedef I3EventInfoConverter<I3RecoPulse> I3EventInfoConverterFromRecoPulses;
    I3CONVERTER_EXPORT(I3EventInfoConverterFromRecoPulses,"Dumps event information calculated from I3RecoPulses");
    typedef I3EventInfoConverter<I3MCHit> I3EventInfoConverterFromMCHit;
    I3CONVERTER_EXPORT(I3EventInfoConverterFromMCHit,"Dumps event information calculated from I3MCHit");
    
    I3CONVERTER_EXPORT_DEFAULT(I3CutValuesConverter, "Dumps I3CutValues verbatim");

    // I3RecoInfoConverter has a non-default constructor
    // don't register this converter, since registered converters need default constructors
    bp::class_<I3RecoInfoConverter,                                      
    boost::shared_ptr<I3RecoInfoConverter>,                 
    bp::bases<I3Converter>,                      
    boost::noncopyable >("I3RecoInfoConverter", 
                         "calculates number of direct hits and icecube scale cut variables", 
                         bp::init<std::string, int, int>((bp::arg("pulse_map_name"),
					    bp::arg("icecube_config")=(int)I3ScaleCalculator::IC_GUESS,
					    bp::arg("icetop_config")=(int)I3ScaleCalculator::IT_GUESS)))
    ;
};
