/*
 * Copyright  (C) 2010 The Icecube Collaboration
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * @author Fabian Kislat <fabian.kislat@desy.de> Last changed by: $LastChangedBy$
 */

#ifndef ROOTWRITER_I3ROOTBRANCHWRAPPER_H_INCLUDED
#define ROOTWRITER_I3ROOTBRANCHWRAPPER_H_INCLUDED

#include <icetray/I3PointerTypedefs.h>

#include <boost/shared_ptr.hpp>
#include <cassert>
#include <vector>


I3_FORWARD_DECLARATION(I3TableRow);

struct I3Datatype;
class TBranch;
class TTree;


class I3ROOTBranchWrapper {
public:
  I3ROOTBranchWrapper();
  I3ROOTBranchWrapper(TTree *tree, unsigned int index, size_t arrayLength = 1,
		      bool multirow = false);
  I3ROOTBranchWrapper(const I3ROOTBranchWrapper &rhs);
  virtual ~I3ROOTBranchWrapper();

  virtual void Fill(const I3TableRowConstPtr &data) = 0;
  const TBranch *Branch() const { return branch_; }
  TBranch* Branch() { return branch_; }

protected:
  TTree *tree_;
  TBranch *branch_;
  unsigned int index_;
  size_t arrayLength_;
  bool multirow_;

  void SetBranch(TBranch *branch) { branch_ = branch; }
};

I3_POINTER_TYPEDEFS(I3ROOTBranchWrapper);

#endif // ROOTWRITER_I3ROOTBRANCHWRAPPER_H_INCLUDED
