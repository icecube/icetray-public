/**
 *  Copyright (C) 2007
 *  John Pretz
 *  Phil Roth
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
#ifndef ICETRAY_I3ICEPICK_H_INCLUDED
#define ICETRAY_I3ICEPICK_H_INCLUDED

#include <icetray/I3Context.h>
#include <icetray/I3Configuration.h>
#include <icetray/I3ServiceBase.h>
#include <icetray/I3Frame.h>

#include <boost/python.hpp>

/**
 * @brief This class is the base 'selection' class.  Provides the same
 * 'configuration' interface that an I3Module does so you can configure
 * IcePicks.
 *
 * It is expected that every new kind of selection would inherit from 
 * this class.  Currently, the user would then be able to use their selection
 * in three ways: I3IcePickModule<XXX>, I3IceForkModule<XXX>, and I3IcePickInstaller<XXX>
 * where XXX is your new kind of selection.
 */
class I3IcePick : public I3ServiceBase
{
 public:
  /**
   * @brief the constructor takes an I3Context as an argument.
   */
  I3IcePick(const I3Context& context);

  /**
   * @brief This function configures the parameters that the I3IcePick
   * base class containes.  From here, Configure is then called to give
   * the derived classes a chance to configure themselves.
   */
  void ConfigureInterface();
  
  /**
   * @brief This can be overridden like for I3Module's  It is called 
   * after the constructor but before the 'SelectFrame' method
   * so that these can be configured like I3Modules
   */
  virtual void Configure();

  virtual ~I3IcePick();

  /**
   * @brief This function is meant to be the interface between the users
   * of the IcePick and the derived IcePicks.  Here, quantities important to 
   * the base class, I3IcePick, are dealt with before the derived classes
   * SelectFrame() method is called.
   */
  bool SelectFrameInterface(I3Frame& frame);

  /**
   * @brief This is the meaty method.  Derived classes need to write
   * this method.  It should return 'true' if the event is interesting.
   */
  virtual bool SelectFrame(I3Frame& frame) = 0;

  /**
   * @brief Simple method to get the number of frames deemed interesting
   * by this IcePick
   */
  unsigned GetFramesPassed(){ return npassed_; };

  /**
   * @brief Simple method to get the number of frames deemed not interesting
   * by this IcePick
   */
  unsigned GetFramesFailed(){ return nfailed_; };

 private:
  template <class A> friend class I3IcePickModule;
  template <class A> friend class I3IceForkModule;
  bool cache_;
  unsigned npassed_;
  std::string cachename_;
  unsigned nfailed_;
};

I3_POINTER_TYPEDEFS(I3IcePick);

#endif
