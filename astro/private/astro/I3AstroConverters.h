#ifndef I3_ASTRO_CONVERTERS_H
#define I3_ASTRO_CONVERTERS_H
/**
 * @brief Tableio Converter for I3Particle to store 
 * astronomical data
 *
 * @copyright (C) 2015 The IceCube Collaboration
 * 
 * $Id$
 *
 * @file I3AstroConverter.cxx
 * @author Kevin Meagher
 * @date August 2015
 * $Rev$
 * $LastChangedBy$
 * $LastChangedDate$
 *
 */
#include "tableio/I3Converter.h"
#include "dataclasses/physics/I3EventHeader.h"

class I3Particle;

class I3AstroConverter : public I3ConverterImplementation<I3Particle> {
 public:
  I3AstroConverter(const std::string &eventHeaderName = I3DefaultName<I3EventHeader>::value())
   : eventHeaderName_(eventHeaderName)
  {}
  
 private:
  I3TableRowDescriptionPtr CreateDescription(const I3Particle&); 
  size_t FillRows(const I3Particle &particle, I3TableRowPtr rows);

  std::string eventHeaderName_;

  SET_LOGGER("I3AstroConverter");
};

class I3GalacticConverter : public I3ConverterImplementation<I3Particle> {
 public:
  I3GalacticConverter(const std::string &eventHeaderName = I3DefaultName<I3EventHeader>::value())
   : eventHeaderName_(eventHeaderName)
  {}
  
 private:
  I3TableRowDescriptionPtr CreateDescription(const I3Particle&); 
  size_t FillRows(const I3Particle &particle, I3TableRowPtr rows);

  std::string eventHeaderName_;

  SET_LOGGER("I3GalacticConverter");
};

class I3SuperGalacticConverter : public I3ConverterImplementation<I3Particle> {
 public:
  I3SuperGalacticConverter(const std::string &eventHeaderName = I3DefaultName<I3EventHeader>::value())
   : eventHeaderName_(eventHeaderName)
  {}
  
 private:
  I3TableRowDescriptionPtr CreateDescription(const I3Particle&); 
  size_t FillRows(const I3Particle &particle, I3TableRowPtr rows);

  std::string eventHeaderName_;

  SET_LOGGER("I3GalacticConverter");
};

class I3SunAndMoonConverter : public I3ConverterImplementation<I3EventHeader> {
 public:
  I3SunAndMoonConverter() {}
  
 private:
  I3TableRowDescriptionPtr CreateDescription(const I3EventHeader&); 
  size_t FillRows(const I3EventHeader &header, I3TableRowPtr rows);

  SET_LOGGER("I3SunAndMoonConverter");
};
#endif //I3_ASTRO_CONVERTERS_H
