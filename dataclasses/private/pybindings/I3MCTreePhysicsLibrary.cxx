//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id: I3MCTree.cxx 126481 2014-12-02 22:45:21Z david.schultz $
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

#include <boost/python.hpp>
#include "I3MCTreePhysicsLibraryWrappers.hh"

namespace bp = boost::python;

void register_I3MCTreePhysicsLibrary()
{
  bp::def("get_most_energetic_primary", get_most_energetic_primary, "Returns the most energetic primary.");
  bp::def("get_most_energetic_inice", &get_most_energetic_inice, "Returns the more energetic InIce primary.");
  bp::def("get_most_energetic", &get_most_energetic, "Returns the most energetic I3Particle.");
  bp::def("get_most_energetic_track", &get_most_energetic_track, "Returns the most energetic track.");
  bp::def("get_most_energetic_cascade", &get_most_energetic_cascade, "Returns the most energetic cascade.");
  bp::def("get_most_energetic_inice_cascade", &get_most_energetic_inice_cascade, "Returns the most energetic InIce cascade.");
  bp::def("get_most_energetic_neutrino", &get_most_energetic_neutrino, "Returns the most energetic neutrino.");
  bp::def("get_most_energetic_muon", &get_most_energetic_muon, "Returns the most energetic muon.");
  bp::def("get_most_energetic_nucleus", &get_most_energetic_nucleus, "Returns the most energetic nucleus.");
}

