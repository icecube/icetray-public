/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *  
 *  SPDX-License-Identifier: BSD-2-Clause
 *  
 */

#ifndef I3DEFAULTNAME_H_INCLUDED
#define I3DEFAULTNAME_H_INCLUDED

#include <boost/shared_ptr.hpp>
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
