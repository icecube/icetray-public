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

#include <I3Test.h>

#include "tableio/converter/I3VectorConverter.h"
#include "tableio/converter/I3ParticleConverter.h"

#include "dataclasses/physics/I3Particle.h"
#include "dataclasses/I3Constants.h"
#include "icetray/I3Frame.h"

typedef I3VectorConverter< I3ParticleConverter > I3VectorI3ParticleConverter;
typedef I3Vector<I3Particle> I3VectorI3Particle;
I3_POINTER_TYPEDEFS(I3VectorI3Particle);


TEST_GROUP(I3VectorI3ParticleConverterTest)


TEST(description_from_empty) {
  I3ConverterPtr converter(new I3VectorI3ParticleConverter);
    
  I3VectorI3ParticlePtr particle_vect(new I3VectorI3Particle);
  ENSURE_EQUAL( converter->GetNumberOfRows(particle_vect), size_t(0), "Vector is empty");
    
  I3TableRowDescriptionConstPtr desc = converter->GetDescription(particle_vect);
  ENSURE( desc != 0, "converter returned non-null pointer.");
}


TEST(description) {
  I3ConverterPtr converter(new I3VectorI3ParticleConverter);

  I3VectorI3ParticlePtr particle_vect(new I3VectorI3Particle);
  particle_vect->push_back(I3Particle());
  ENSURE_EQUAL( converter->GetNumberOfRows(particle_vect), size_t(1), "There is one element in the vector");
    
  I3TableRowDescriptionConstPtr desc = converter->GetDescription(particle_vect);
  ENSURE( desc != 0, "converter returned non-null pointer.");
}


TEST(type_id) {
  I3ConverterPtr converter(new I3VectorI3ParticleConverter);
  I3VectorI3ParticlePtr particle_vect(new I3VectorI3Particle);

  ENSURE( converter->CanConvert(particle_vect), "Converter recognizes its own.");
}


TEST(assignment) {
  I3ConverterPtr converter(new I3VectorI3ParticleConverter);

  I3VectorI3ParticlePtr particle_vect(new I3VectorI3Particle);
  I3FramePtr frame(new I3Frame);   // a fake frame
  
  I3Particle particle;
  particle.SetType(I3Particle::Monopole);
  particle.SetShape(I3Particle::TopShower);   // this is nonsense ... on purpose
  particle.SetFitStatus(I3Particle::GeneralFailure);
  particle.SetLocationType(I3Particle::Anywhere);
  particle.SetPos(42, 42, 42);
  particle.SetDir(1, 0);
  particle.SetTime(2012);
  particle.SetLength(1);
  particle.SetEnergy(100);
  particle.SetSpeed(I3Constants::c);
  particle_vect->push_back(particle);

  frame->Put("ParticleList", particle_vect);

  I3TableRowDescriptionConstPtr desc = converter->GetDescription(particle_vect);
  I3TableRowPtr rows(new I3TableRow(desc, converter->GetNumberOfRows(particle_vect)));
  converter->Convert(particle_vect, rows, frame);
    
  ENSURE_EQUAL( rows->Get<double>("x"), 42.0);
  ENSURE_EQUAL( rows->Get<double>("y"), 42.0);
  ENSURE_EQUAL( rows->Get<double>("z"), 42.0);

  ENSURE_EQUAL( rows->Get<double>("time"), 2012);

  ENSURE_EQUAL( rows->Get<double>("zenith"), 1);
  ENSURE_EQUAL( rows->Get<double>("azimuth"), 0);

  ENSURE_EQUAL( rows->Get<double>("energy"), 100);
  ENSURE_EQUAL( rows->Get<double>("speed"), I3Constants::c);
  ENSURE_EQUAL( rows->Get<double>("length"), 1);

  ENSURE_EQUAL( rows->Get<I3Particle::ParticleType>("type"), I3Particle::Monopole);
  ENSURE_EQUAL( rows->Get<I3Particle::ParticleShape>("shape"), I3Particle::TopShower);
  ENSURE_EQUAL( rows->Get<I3Particle::LocationType>("location"), I3Particle::Anywhere);
  ENSURE_EQUAL( rows->Get<I3Particle::FitStatus>("fit_status"), I3Particle::GeneralFailure);
    
}

