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

#include <I3Test.h>
#include <boost/preprocessor/stringize.hpp>
#include "icetray/I3Tray.h"
#include "icetray/I3ServiceFactory.h"
#include <icetray/I3Frame.h>
#include <icetray/OMKey.h>
#include <boost/assign/std/vector.hpp>

using namespace boost::assign;

TEST_GROUP(OMKey);

TEST(hash)
{
#ifdef I3_USE_FAST_OMKEY_MAP
  vector<int> hashes((80+19) * 64);
  for (int string = -19; string < 80; string++)
    for (int omnum = 0; omnum < 64; omnum++)
      hashes[OMKey::hash()(OMKey(string, omnum))]++;

  for (int i = 0; i< hashes.size(); i++)
    {
      std::cout << hashes[i] << " ";
      if (hashes[i] > 1)
	log_fatal("collision at %d", i);
    }
#endif
}


