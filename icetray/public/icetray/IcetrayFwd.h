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

#ifndef ICETRAYFWD_H_INCLUDED
#define ICETRAYFWD_H_INCLUDED

#include <stdint.h> //int64_t, etc

#include <icetray/I3Logging.h>
#include <icetray/I3PointerTypedefs.h>

namespace boost {
  namespace serialization {
    class access;
  }
  namespace python { 
    namespace api
    {
      class object;
    }
    using api::object;
  }
}

namespace I3 {
  namespace Tray {
    namespace Config {
      class Service;
    }
  }
}

class I3Module;  I3_POINTER_TYPEDEFS(I3Module);
class I3Context;  I3_POINTER_TYPEDEFS(I3Context);
class I3ServiceFactory; I3_POINTER_TYPEDEFS(I3ServiceFactory);
class I3Configuration; I3_POINTER_TYPEDEFS(I3Configuration);
class I3Frame; I3_POINTER_TYPEDEFS(I3Frame);

#endif 

