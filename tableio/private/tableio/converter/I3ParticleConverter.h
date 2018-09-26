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

#ifndef TABLEIO_I3PARTICLECONVERTER_H_INCLUDED
#define TABLEIO_I3PARTICLECONVERTER_H_INCLUDED

#include "tableio/I3Converter.h"
#include "dataclasses/physics/I3Particle.h"

class I3ParticleConverter : public I3ConverterImplementation<I3Particle> {
private:
    I3TableRowDescriptionPtr CreateDescription(const I3Particle& particle);
    size_t FillRows(const I3Particle& particle, I3TableRowPtr rows);
};

#endif // TABLEIO_I3PARTICLECONVERTER_H_INCLUDED
