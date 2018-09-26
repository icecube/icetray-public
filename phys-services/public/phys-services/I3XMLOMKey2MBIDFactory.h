/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3XMLOMKey2MBIDFactory.h
 * @version $Revision$
 * @date $Date$
 * @author tschmidt
 */
#ifndef I3XMLOMKEY2MBIDFACTORY_H_INCLUDED
#define I3XMLOMKEY2MBIDFACTORY_H_INCLUDED

// forward declarations

class I3Context;

// header files

#include <string>

#include <icetray/I3Logging.h>
#include <icetray/I3ServiceFactory.h>
#include <interfaces/I3OMKey2MBID.h>

// definitions


/**
 * @brief This class installs an I3XMLOMKey2MBID service.
 * 
 * I3XMLOMKey2MBIDFactory supports two parameter:
 * <ul>
 * <li><VAR>Infile</VAR> (the name of the file to get the mainboard ID list from) and
 * <li><VAR>InstallServiceAs</VAR> (install the service at the following location
 * (default value is the value according to I3_DEFAULT_NAME)).
 * </ul>
 */
class I3XMLOMKey2MBIDFactory : public I3ServiceFactory
{
public:
  /**
   * @brief Constructor.
   * 
   * @param context The context in which the service will exist.
   */
  explicit I3XMLOMKey2MBIDFactory(const I3Context& context);
  /**
   * @brief Destructor.
   */
  virtual ~I3XMLOMKey2MBIDFactory();
  bool InstallService(I3Context& services);
  void Configure();

private:
  I3OMKey2MBIDPtr service_;
  std::string infile_;  
  std::string installServiceAs_;
  
  
  // private copy constructors and assignment
  I3XMLOMKey2MBIDFactory(const I3XMLOMKey2MBIDFactory&);
  I3XMLOMKey2MBIDFactory& operator=(const I3XMLOMKey2MBIDFactory&);


  // logging
  SET_LOGGER ("I3XMLOMKey2MBIDFactory");
};

#endif /*I3XMLOMKEY2MBIDFACTORY_H_*/
