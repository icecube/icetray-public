//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id$
//
//   This file is part of IceTray.
//
//   IceTray is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
//   IceTray is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <vector>

#include <dataclasses/I3DOMFunctions.h>

using namespace boost::python;

void register_I3DOMFunctions()
{

  def("atwd_sampling_rate",&ATWDSamplingRate);
  def("pmt_gain",&PMTGain);
  def("spe_mean",&SPEMean);
  def("mean_spe_charge",&MeanSPECharge);
  def("fadc_baseline",&FADCBaseline);
  def("transit_time",&TransitTime);
  def("spe_discriminator_threshold",&SPEDiscriminatorThreshold);
  def("spe_pmt_threshold",&SPEPMTThreshold);
  def("mpe_discriminator_threshold",&MPEDiscriminatorThreshold);
  def("domcaal_version",&DOMCalVersion);
  def("which_atwd",&WhichATWD);
  def("old_mpe_threshold",&OldmpeThreshold);
  def("old_spe_threshold",&OldspeThreshold);

} 
