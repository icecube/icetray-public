#ifndef I3FILEOMKEY2MBIDFACTORY_H
#define I3FILEOMKEY2MBIDFACTORY_H

class I3Context;

#include <string>
#include <icetray/I3ServiceFactory.h>
#include <interfaces/I3OMKey2MBID.h>


class I3FileOMKey2MBIDFactory
: public I3ServiceFactory
{
 public:

  I3FileOMKey2MBIDFactory(const I3Context& context);

  virtual ~I3FileOMKey2MBIDFactory(){}

  bool InstallService(I3Context& services);

  void Configure();

 private:
  std::string infile_;

  I3OMKey2MBIDPtr service_;

  I3FileOMKey2MBIDFactory
    (const I3FileOMKey2MBIDFactory& rhs); 
  I3FileOMKey2MBIDFactory operator=
    (const I3FileOMKey2MBIDFactory& rhs); 

 SET_LOGGER("I3FileOMKey2MBIDFactory");

};

#endif 
