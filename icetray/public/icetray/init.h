/**
 *  $Id$
 *  
 *  Copyright (C) 2012
 *  the IceCube Collaboration <http://www.icecube.wisc.edu>
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
#ifndef ICETRAY_INIT_H_INCLUDED
#define ICETRAY_INIT_H_INCLUDED

namespace I3{
    /**
     * Initialize the icetray library.  This function will initialize
     * Python, if not already done, and load the icetray module into it;
     * this ensures that icetray code that interacts with python data
     * structures will work correctly.
     *
     * This function should be called near the beginning of any C++
     * program that uses libicetray.  Python scripts don't need to
     * worry about this.
     * */
    void init_icetray_lib();
}

#endif /* ICETRAY_INIT_H_INCLUDED */
