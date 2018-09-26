#ifndef I3STRINGADDERSERVICEFACTORY_H
#define I3STRINGADDERSERVICEFACTORY_H

#include "icetray/I3ServiceFactory.h"

class I3StringAdderService;
class I3OMGeo;
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
class I3StringAdderServiceFactory : public I3ServiceFactory
{
public:

  /*
   * Constructor.
   */ 
  I3StringAdderServiceFactory(const I3Context& ctx);
  
  /*
   * Destructor.
   */ 
  virtual ~I3StringAdderServiceFactory();
  
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
  I3StringAdderServiceFactory();
  I3StringAdderServiceFactory(const I3StringAdderServiceFactory& source);
  I3StringAdderServiceFactory& operator=(const I3StringAdderServiceFactory& source);

  boost::shared_ptr<I3StringAdderService> geometry_;

  /**
   * List of top (i.e. OMNumber = 1) DOM positions.
   * This will be built from the rest of the parameters.
   */
  std::vector<I3OMGeo> omGeoList_;

  /**
   * Parameter - List of x positions 
   * Default value is 'empty'
   */
  std::vector<double> xPositions_;

  /**
   * Parameter - List of y positions
   * Default value is 'empty'
   */
  std::vector<double> yPositions_;

  /**
   * Parameter - Depth of the top DOMs
   * Default value is +500 * I3Units::m
   */
  double depth_;  

  /**
   * Parameter - Distance between the DOMs
   * Default value is 17 * I3Units::m
   */
  double domSpacing_;

  /**
   * Parameter - Number of DOMs per string
   * Default value is 60
   */
  unsigned int domsPerString_;

  /**
   * Parameter - Orientation of the DOM
   * Default value is -1 (i.e. Down)
   * Unspecified=0, Up=1, Down=-1
   */
  int domOrientation_;

  /**
   * Parameter - Type of DOM
   * Default value is 20 (i.e. IceCube)
   * AMANDA = 10, IceCube = 20, IceTop = 30
   */
  int domType_;

  /**
   * Parameter - Effective collection area
   * Default value is .0444 * I3Units::m2 
   * I don't think this is used
   */
  double area_;

  /**
   * Parameter - relative rotation angle of DOM in azimuth
   * Default value is 0.
   * I don't think this is used anywhere.
   */
  double aziangle_;

  /**
   *Name of the new geometry service.  The muxer needs this name.
   */
  std::string geoSelectorName_;

  /**
   *Name of the previous geometry service.
   */
  std::string geoServiceName_;

  SET_LOGGER("I3StringAdderServiceFactory");
  
};  // end of class I3StringAdderServiceFactory

#endif 
