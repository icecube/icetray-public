/*
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef ROOTWRITER_ONLYFORTEST_H_INCLUDED
#define ROOTWRITER_ONLYFORTEST_H_INCLUDED

#include "tableio/rootwriter/I3ROOTBranchWrapperEnum.h"

class OnlyForTest
{
  I3ROOTBranchWrapperEnumPtr wrapperenum_;
  
 public:
 OnlyForTest(const I3ROOTBranchWrapperEnumPtr &wrapperenum):
  wrapperenum_(wrapperenum){};
  
  ~OnlyForTest(){};
  
  I3ROOTBranchWrapperEnum::FillFn
    GetForTestselectFillImp(const I3Datatype &type)
    {
      return wrapperenum_->selectFillImp(type);
    }
 
  template <typename T>
    void ForTestFillImp(const I3TableRowConstPtr &data)
    {
      wrapperenum_->FillImp<T>(data);
    }
 
};

#endif 
