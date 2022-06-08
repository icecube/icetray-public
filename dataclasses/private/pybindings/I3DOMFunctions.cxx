//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id$
//
//   This file is part of IceTray.
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//   
//   THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
//   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
//   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
//   OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
//   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
//   SUCH DAMAGE.
//   
//   SPDX-License-Identifier: BSD-2-Clause
//   
//

#include <vector>

#include <dataclasses/I3DOMFunctions.h>

using namespace boost::python;

BOOST_PYTHON_FUNCTION_OVERLOADS(SPEDiscriminatorThreshold_overloads, SPEDiscriminatorThreshold, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(MPEDiscriminatorThreshold_overloads, MPEDiscriminatorThreshold, 2, 3)

void register_I3DOMFunctions()
{

  def("atwd_sampling_rate",&ATWDSamplingRate);
  def("pmt_gain",&PMTGain);
  def("spe_mean",&SPEMean);
  def("mean_spe_charge",&MeanSPECharge);
  def("fadc_baseline",&FADCBaseline);
  def("transit_time",&TransitTime);
  def("spe_discriminator_threshold",&SPEDiscriminatorThreshold, SPEDiscriminatorThreshold_overloads(args("status", "calib", "spePeakVoltageChargeRatio"), ""));
  def("spe_pmt_threshold",&SPEPMTThreshold);
  def("mpe_discriminator_threshold",&MPEDiscriminatorThreshold, MPEDiscriminatorThreshold_overloads(args("status", "calib", "spePeakVoltageChargeRatio"), ""));
  def("domcaal_version",&DOMCalVersion);
  def("which_atwd",&WhichATWD);
  def("old_mpe_threshold",&OldmpeThreshold);
  def("old_spe_threshold",&OldspeThreshold);

} 
