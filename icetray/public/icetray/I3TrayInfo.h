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
#ifndef ICETRAY_I3TRAYINFO_H_INCLUDED
#define ICETRAY_I3TRAYINFO_H_INCLUDED

#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include <icetray/I3Configuration.h>
#include <icetray/I3FrameObject.h>

struct I3TrayInfo : public I3FrameObject
{
  std::map<std::string, std::string> host_info;

  std::string svn_url, svn_externals;
  unsigned svn_revision;

  std::vector<std::string> modules_in_order;
  std::vector<std::string> factories_in_order;

  std::map<std::string, I3ConfigurationPtr> module_configs;
  std::map<std::string, I3ConfigurationPtr> factory_configs;
	
  template <class Archive>
  void 
  serialize (Archive & ar, unsigned version);
  
  std::ostream& Print(std::ostream&) const override;

  ///Print the factory/module parameters in a way which is more compact than the
  ///normal print method
  std::string PrintCompact();

  I3TrayInfo();

};

std::ostream& 
operator<<(std::ostream& os, const I3TrayInfo& config);

I3_DEFAULT_NAME(I3TrayInfo);
I3_POINTER_TYPEDEFS(I3TrayInfo);
I3_CLASS_VERSION(I3TrayInfo, 2);
#endif


