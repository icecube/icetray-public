/**
 * copyright  (C) 2004
 * the IceCube collaboration
 *  $Id$
  *
 * @file I3GeoSelTestModule.h
 * @version $Revision$
 * @date $Date$
 * @author olivas
 */

#ifndef GEOSELTESTMODULE_H
#define GEOSELTESTMODULE_H

// header files
#include "icetray/I3Module.h"

/**
 * @brief IceTray module to test whether I3GeometrySelector is working properly.
 *The two parameters stringsToUse_ and stringsToExclude_ should be set to the same 
 *values as the I3GeometrySelector.  This test passes if the geometry contains only OMs
 *that are in stringsToUse_ and does not contain any DOMs that are in stringsToExclude_.
 */

//This needs to be turned into a service that gets the geometry from another service
//and not the frame.  The muxer can then be pointed to this service to get the trimmed geometry.

class I3GeoSelTestModule : public I3Module
{
public:

  /*
   * Constructor:  builds an instance of the module, with the
   * context provided by IceTray. 
   */ 
  I3GeoSelTestModule(const I3Context& ctx);
  
  /*
   * Destructor: deletes the module
   */ 
  ~I3GeoSelTestModule();
  
  /**
   * This module takes a configuration parameter and so it must be configured
   */
  void Configure();
  
  /** 
   * We'll want to trim the geometry everytime it gets updated.
   */ 
  void Geometry(I3FramePtr frame);

  void Physics(I3FramePtr frame);

private:
  
  /**
   *default constructor, assignment operator, and copy constructor
   * declared private to prevent use
   */
  I3GeoSelTestModule();
  I3GeoSelTestModule(const I3GeoSelTestModule& source);
  I3GeoSelTestModule& operator=(const I3GeoSelTestModule& source);

  /**
   *Takes the form of "i,j,n:m" and this will use strings i,j,n,n+1,...,m-1,m
   */
  std::string stringsToUse_;

  /**
   *Takes the form of "i,j,n:m" and this will exclude strings i,j,n,n+1,...,m-1,m
   */
  std::string stringsToExclude_;

  /**
   *Takes the form of "i,j,n:m" and this will use stations i,j,n,n+1,...,m-1,m
   */
  std::string stationsToUse_;

  /**
   *Takes the form of "i,j,n:m" and this will exclude stations i,j,n,n+1,...,m-1,m
   */
  std::string stationsToExclude_;

  double shiftX_;
  double shiftY_;
  double shiftZ_;

  bool detectorCentered_;
  /**
   *This is the list of strings to use
   */
  std::vector<int> goodStrings_;
  std::vector<int> goodStations_;

  // Named stream for my logging messages.  Allows my messages to be
  // sent to a particular log file, to be set to a special threshold, etc.
  SET_LOGGER("I3GeoSelTestModule");
  
};  // end of class I3GeoSelTestModule

#endif //I3GEOSELTESTMODULE_H
