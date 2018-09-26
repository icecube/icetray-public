#ifndef I3GEOMETRYSELECTORSERVICEFACTORY_H
#define I3GEOMETRYSELECTORSERVICEFACTORY_H

#include "icetray/I3ServiceFactory.h"

class I3GeometrySelectorService;
/**
 * @brief IceTray module that selects the strings to use.

 * This module has only two parameters "StringsToUse" and "StringsToExclude"
 * The default values for StringsToUse is the entire 80 string IceCube 
 * detector and AMANDA (i.e. "-19:80").  No strings are excluded by default.
 *
 * Examples values for the parameters are...
 *
 * 1) Use only the IceCube detector.
 * Either set StringsToUse to "1:80" or set StringsToExclude to "-19:0" 
 * (remember the default value for StringsToUse is "-19:80").
 *
 * 2) Use the current geometry (as of mid Jan '06).
 * Set StringsToUse to "21,29,30,38,39,-19:-1"
 */
class I3GeometrySelectorServiceFactory : public I3ServiceFactory
{
public:

  /*
   * Constructor.
   */ 
  I3GeometrySelectorServiceFactory(const I3Context& ctx);
  
  /*
   * Destructor.
   */ 
  virtual ~I3GeometrySelectorServiceFactory();
  
  /**
   * Checks the two parameters StringsToUse and StringsToExclude are valid.
   */
  virtual void Configure();
  
  virtual bool InstallService(I3Context& services);

private:

  /**
   *default constructor, assignment operator, and copy constructor
   * declared private to prevent use
   */
  I3GeometrySelectorServiceFactory();
  I3GeometrySelectorServiceFactory(const I3GeometrySelectorServiceFactory& source);
  I3GeometrySelectorServiceFactory& operator=(const I3GeometrySelectorServiceFactory& source);

  boost::shared_ptr<I3GeometrySelectorService> geometry_;
  /**
   * Parameter - The strings to use.
   * Default value is "-19:80".
   */
  std::string stringsToUse_;

  /**
   * Parameter - The strings to exclude.
   * Default value is NULL.
   */
  std::string stringsToExclude_;

  /**
   * Parameter - The stations to use.
   * Default value is "-19:80".
   */
  std::string stationsToUse_;

  /**
   * Parameter - The stations to exclude.
   * Default value is NULL.
   */
  std::string stationsToExclude_;

  /**
   * Distance in X to shift the detector
   */
  double shiftX_;

  /**
   * Distance in Y to shift the detector
   */
  double shiftY_;

  /**
   * Distance in Z to shift the detector
   */
  double shiftZ_;

  /**
   * Shift to the center of the detector in X-Y
   */
  bool shiftToCenter_;

  /**
   * A list of strings to use.  This vector is constructed from the input parameters
   * StringsToUse and StringsToExclude.
   */
  std::vector<int> goodStrings_;

  /**
   * A list of strings to use.  This vector is constructed from the input parameters
   * StringsToUse and StringsToExclude.
   */
  std::vector<int> goodStations_;

  /**
   *Name of the new geometry service.  The muxer needs this name.
   */
  std::string geoSelectorName_;

  /**
   *Name of the previous geometry service.
   */
  std::string geoServiceName_;

  SET_LOGGER("I3GeometrySelectorServiceFactory");
  
};  // end of class I3GeometrySelectorServiceFactory

#endif 
