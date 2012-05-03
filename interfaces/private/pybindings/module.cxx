/**
 *  $Id$
 *  
 *  Copyright (C) 2008
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


void register_I3CalibrationService();
void register_I3DetectorStatusService();
void register_I3GeometryService();
void register_I3EventService();
void register_I3MetaService();
void register_I3OMKey2MBID();

BOOST_PYTHON_MODULE(interfaces)
{

  register_I3CalibrationService();
  register_I3DetectorStatusService();
  register_I3GeometryService();
  register_I3EventService();
  register_I3MetaService();
  register_I3OMKey2MBID();
}
