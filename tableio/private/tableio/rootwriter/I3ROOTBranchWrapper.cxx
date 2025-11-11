/*
 * Copyright  (C) 2010 The Icecube Collaboration
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * @author Fabian Kislat <fabian.kislat@desy.de> Last changed by: $LastChangedBy$
 */

#include "I3ROOTBranchWrapper.h"

I3ROOTBranchWrapper::I3ROOTBranchWrapper()
  : tree_(0), branch_(0), index_(0), multirow_(false)
{}

I3ROOTBranchWrapper::I3ROOTBranchWrapper(TTree *tree, unsigned int index,
					 size_t arrayLength, bool multirow)
  : tree_(tree), branch_(0), index_(index), arrayLength_(arrayLength),
    multirow_(multirow)
{}

I3ROOTBranchWrapper::I3ROOTBranchWrapper(const I3ROOTBranchWrapper &rhs)
  : tree_(rhs.tree_), branch_(rhs.branch_), index_(rhs.index_),
    arrayLength_(rhs.arrayLength_), multirow_(rhs.multirow_)
{}

I3ROOTBranchWrapper::~I3ROOTBranchWrapper() {}
