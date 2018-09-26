/**
 * copyright  (C) 2007
 * the icecube collaboration
 * $Id$
 *
 * @version $Revision$
 * @date $Date$
 * @author juancarlos@icecube.wisc.edu
 *
 */

#ifndef I3SUMMARYSERVICE_H
#define I3SUMMARYSERVICE_H

#include <icetray/I3Logging.h>
#include <icetray/I3DefaultName.h>
#include <icetray/I3PointerTypedefs.h>
#include <dataclasses/I3Map.h>

/**
 * @brief This is an interface for services which allow modules to store 
 * runtimejob statistics that can then be written to external xml files
 * for other programs to parse or upload values to a database, etc. useful for
 * collecting statistics in production runs.
 */

class I3SummaryService: public I3Map<std::string,double>
{
 public:
  /**
   * constructor
   */
  I3SummaryService() {}


  /**
   * destructor
   */ 
  virtual ~I3SummaryService() {};


  /**
   * Access dictionary entry
   */
  virtual double& operator[] (const std::string& key)=0;

  /**
   * Add a value to the value summary map
   */
  virtual void SetValue(std::string name, double value)=0;

  /**
   * Get value from the value summary map
   */
  virtual double GetValue(std::string name)=0;


 private:

  // copy and assignment private
  I3SummaryService(const I3SummaryService&);
  const I3SummaryService& operator=(const I3SummaryService&);

  SET_LOGGER("I3SummaryService");

};

I3_DEFAULT_NAME(I3SummaryService);
I3_POINTER_TYPEDEFS(I3SummaryService);

#endif //I3SUMMARYSERVICE_H
