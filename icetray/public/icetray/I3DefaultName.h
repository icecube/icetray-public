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

#ifndef I3DEFAULTNAME_H_INCLUDED
#define I3DEFAULTNAME_H_INCLUDED

#include <boost/preprocessor/stringize.hpp>

template <typename T>
struct I3DefaultName
{

};

template <typename T> 
struct I3DefaultName<const T>
{				
  const static char *value()	
  {				
    return I3DefaultName<T>::value();
  }				
};

template <typename T>
struct I3DefaultName<boost::shared_ptr<T> >
{
  const static char *value()
  {
    return I3DefaultName<T>::value();
  }
};

template <typename T>
struct I3DefaultName<const boost::shared_ptr<T> >
{
  const static char *value()
  {
    return I3DefaultName<T>::value();
  }
};

template <typename T>
struct I3DefaultName<boost::shared_ptr<const T> >
{
  const static char *value()
  {
    return I3DefaultName<T>::value();
  }
};

template <typename T>
struct I3DefaultName<const boost::shared_ptr<const T> >
{
  const static char *value()
  {
    return I3DefaultName<T>::value();
  }
};


#define I3_DEFAULT_NAME(TYPE)			\
  template <> struct I3DefaultName<TYPE>	\
  {						\
    const static char *value()			\
    {						\
      return BOOST_PP_STRINGIZE(TYPE);		\
    }						\
  };						\

#endif // I3DEFAULTNAME_H_INCLUDED
