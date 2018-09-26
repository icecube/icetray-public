#ifndef I3RECOINFOCONVERTER_H_INCLUDED
#define I3RECOINFOCONVERTER_H_INCLUDED

/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> $LastChangedBy$
 */

#include <tableio/I3Converter.h>
#include <dataclasses/physics/I3Particle.h>

class I3RecoInfoConverter : public I3ConverterImplementation<I3Particle > {
public:
    I3RecoInfoConverter();
    I3RecoInfoConverter(std::string pulseMapName);
    I3RecoInfoConverter(std::string pulseMapName, int icecubeConf_, int icetopConf_);
private:
    I3TableRowDescriptionPtr CreateDescription(const I3Particle& reco);
    size_t FillRows(const I3Particle& reco, I3TableRowPtr rows);

    std::string generateDocString(std::string prefix,
                                  std::string identifier,
                                  bool muon);
    void defineTimeWindows();

    std::string pulseMapName_;
    int icecubeConf_;
    int icetopConf_; 
    std::map<std::string, std::pair<double, double > > timeWindows_;
    std::map<std::string, std::pair<double, double > > muonTimeWindows_;

    SET_LOGGER("I3RecoInfoConverter");
};

#endif  // I3RECOINFOCONVERTER_H_INCLUDED
