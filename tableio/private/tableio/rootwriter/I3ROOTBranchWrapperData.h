/*
 * Copyright  (C) 2010 The Icecube Collaboration
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * @author Fabian Kislat <fabian.kislat@desy.de> Last changed by: $LastChangedBy$
 */

#ifndef ROOTWRITER_I3ROOTBRANCHWRAPPERDATA_H_INCLUDED
#define ROOTWRITER_I3ROOTBRANCHWRAPPERDATA_H_INCLUDED

#include "I3ROOTBranchWrapper.h"
#include <icetray/I3PointerTypedefs.h>

#include <boost/shared_ptr.hpp>
#include <cassert>
#include <vector>


I3_FORWARD_DECLARATION(I3TableRow);

struct I3Datatype;
class TBranch;
class TTree;


class I3ROOTBranchWrapperData : public I3ROOTBranchWrapper {
public:
  I3ROOTBranchWrapperData();
  I3ROOTBranchWrapperData(TTree *tree, const I3Datatype &type, const std::string &branchname,
			  const std::string &docstring, unsigned int index, size_t arrayLength = 1,
			  const I3ROOTBranchWrapperConstPtr &counter = I3ROOTBranchWrapperConstPtr());
  I3ROOTBranchWrapperData(const I3ROOTBranchWrapperData &rhs);
  virtual ~I3ROOTBranchWrapperData();

  template <typename T>
  void Fill(const T &data)
  {
    assert(datasize_ == sizeof(T));

    if (data_->size() < datasize_) {
      char *olddata = &(data_->at(0));
      data_->resize(datasize_);
      if (olddata != &(data_->at(0))) {
	setBranchAddress();
      }
    }
    *(reinterpret_cast<T*>(&data_->at(0))) = data;
  }

  void Fill(const I3TableRowConstPtr &data);

private:
  size_t datasize_;
  boost::shared_ptr< std::vector<char> > data_;
  bool rootCharArrayHack_;
  bool rootCharArrayIsSigned_;
  void setBranchAddress();

  template <typename Source, typename Destination>
  void RootCharArrayHackFillData(const I3TableRowConstPtr &data);
};

I3_POINTER_TYPEDEFS(I3ROOTBranchWrapperData);

#endif // ROOTWRITER_I3ROOTBRANCHWRAPPER_H_INCLUDED
