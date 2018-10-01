/**
 *  $Id$
 *  
 *  Copyright (C) 2007   Troy D. Straszheim  <troy@icecube.umd.edu>
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
#include <icetray/modules/Dump.h>

I3_MODULE(Dump);

#include <iostream>
#include <icetray/I3Context.h>

using namespace std;

Dump::Dump(const I3Context& context)
  : I3ConditionalModule(context),
    physicsPrescale_(1u),
    frameCount_(0u)
{
  AddParameter("Prescale",
               "Reduce the output for physics frames by this prescale factor",
               physicsPrescale_);

  AddOutBox("OutBox");
}

Dump::~Dump()
{
}

void Dump::Configure()
{
  GetParameter("Prescale", physicsPrescale_);
  if(!physicsPrescale_) physicsPrescale_ = 1u;
}

void Dump::Physics(I3FramePtr frame)
{
  frameCount_++;
  if(!(frameCount_ % physicsPrescale_))
  {
    cout<<"Physics.....["<<frameCount_<<"]"<<endl;
    cout<<*frame;
    cout<<"---------------------------------"<<endl;
  }
  PushFrame(frame,"OutBox");
}

void Dump::DetectorStatus(I3FramePtr frame)
{
  frameCount_++;
  cout<<"DetectorStatus.....["<<frameCount_<<"]"<<endl;
  cout<<*frame;
  cout<<"---------------------------------"<<endl;
  PushFrame(frame,"OutBox");
}

void Dump::Geometry(I3FramePtr frame)
{
  frameCount_++;
  cout<<"Geometry.....["<<frameCount_<<"]"<<endl;
  cout<<*frame;
  cout<<"---------------------------------"<<endl;
  PushFrame(frame,"OutBox");
}

void Dump::Calibration(I3FramePtr frame)
{
  frameCount_++;
  cout<<"Calibration.....["<<frameCount_<<"]"<<endl;
  cout<<*frame;
  cout<<"---------------------------------"<<endl;
  PushFrame(frame,"OutBox");
}
