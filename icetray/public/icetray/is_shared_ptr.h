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
#ifndef ICETRAY_IS_SHARED_PTR_H_INCLUDED
#define ICETRAY_IS_SHARED_PTR_H_INCLUDED

#include <boost/mpl/bool.hpp>

/**
   traits classes that identify if a template parameter is a shared_ptr or not
   @tparam T typename to evaluate
*/
template <typename T>
struct is_shared_ptr : public boost::mpl::false_{};

template <typename T>
struct is_shared_ptr<boost::shared_ptr<T> > : public boost::mpl::true_{};

template <typename T>
struct is_shared_ptr<boost::shared_ptr<const T> > : public boost::mpl::true_{};

template <typename T>
struct is_shared_ptr<const boost::shared_ptr<T> > : public boost::mpl::true_{};

template <typename T>
struct is_shared_ptr<const boost::shared_ptr<const T> > : public boost::mpl::true_{};

#endif

