/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *  
 */
#ifndef DATAIO_I3READERSERVICEFACTORY_H_INCLUDED
#define DATAIO_I3READERSERVICEFACTORY_H_INCLUDED

class I3Context;

#include "icetray/I3ServiceFactory.h"
#include "dataio/I3ReaderService.h"

#include <set>

class I3ReaderServiceFactory
: public I3ServiceFactory
{
 public:

  I3ReaderServiceFactory(const I3Context& context);

  virtual ~I3ReaderServiceFactory();

  bool InstallService(I3Context& services);

  void Configure();

 private:

  I3ReaderServiceFactory (const I3ReaderServiceFactory& rhs);
  I3ReaderServiceFactory operator=(const I3ReaderServiceFactory& rhs);

  std::string filename_;
  std::vector<std::string> filenames_;
  
  std::vector<std::string> skip_keys_;

  I3ReaderServicePtr reader_;

  bool omitGeometry_;
  std::string installGeometryServiceAs_;

  bool omitCalibration_;
  std::string installCalibrationServiceAs_;

  bool omitStatus_;
  std::string installDetectorStatusServiceAs_;

  bool omitEvent_;
  std::string installEventServiceAs_;

  bool skip_missingDrivingTime_;

  SET_LOGGER("I3ReaderServiceFactory");
};

#endif
