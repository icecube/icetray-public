/*
 * Copyright  (C) 2010 The Icecube Collaboration
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * @author Fabian Kislat <fabian.kislat@desy.de> Last changed by: $LastChangedBy$
 */

#include "I3ROOTBranchWrapperEnum.h"

#include <icetray/I3Logging.h>
#include <tableio/I3Datatype.h>
#include <tableio/I3TableRow.h>

#include <TBranch.h>
#include <TTree.h>

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#include <cassert>
#include <cctype>
#include <string>


I3ROOTBranchWrapperEnum::I3ROOTBranchWrapperEnum()
  : I3ROOTBranchWrapper(), data_(new std::string(""))
{}

I3ROOTBranchWrapperEnum::I3ROOTBranchWrapperEnum(TTree *tree, const I3Datatype &type,
						 const std::string &branchname,
						 const std::string &docstring,
						 unsigned int index,
						 size_t arrayLength,
						 const I3ROOTBranchWrapperConstPtr &counter)
  : I3ROOTBranchWrapper(tree, index, arrayLength, (bool)counter), data_(new std::string("")),
    fillImp_(selectFillImp(type))
{
  // generate enum value to string mapping
  typedef std::pair<std::string, long> pairstringlong;
  BOOST_FOREACH(pairstringlong enumval, type.enum_members) {
    log_debug("Mapping: %ld --> %s", enumval.second, enumval.first.c_str());
    mapping_[enumval.second] = enumval.first;
  }

  // generate a type description
  std::string leafdescription = branchname;

  // generate description of the array length
  if (multirow_) {
    leafdescription.push_back('[');
    leafdescription.append(counter->Branch()->GetName());
    leafdescription.push_back(']');
  }
  if (arrayLength_ > 1) {
    leafdescription.push_back('[');
    leafdescription.append(boost::lexical_cast<std::string>(arrayLength_));
    leafdescription.push_back(']');
  }

  // generate the type of the field
  leafdescription.push_back('/');
  leafdescription.push_back('C');

  // finally: create the branch
  TBranch *branch = tree->Branch(branchname.c_str(), (void*)data_->c_str(),
				 leafdescription.c_str());
  branch->SetTitle(docstring.c_str());
  SetBranch(branch);
}

I3ROOTBranchWrapperEnum::I3ROOTBranchWrapperEnum(const I3ROOTBranchWrapperEnum &rhs)
  : I3ROOTBranchWrapper(rhs), data_(rhs.data_), mapping_(rhs.mapping_)
{}

I3ROOTBranchWrapperEnum::~I3ROOTBranchWrapperEnum() {}

void I3ROOTBranchWrapperEnum::Fill(const I3TableRowConstPtr &data)
{
  (this->*fillImp_)(data);
}

template <typename T>
void I3ROOTBranchWrapperEnum::FillImp(const I3TableRowConstPtr &data)
{
  *data_ = "";
  for (size_t row = 0; row < data->GetNumberOfRows(); ++row) {
    *data_ += mapping_[*static_cast<const T*>(data->GetPointerToField(index_, row))] + '\0';
  }
  branch_->SetAddress(&(data_->at(0)));
}

I3ROOTBranchWrapperEnum::FillFn I3ROOTBranchWrapperEnum::selectFillImp(const I3Datatype &type)
{
  switch (type.size) {
  case 1:
    return type.is_signed ?
      (FillFn)(&I3ROOTBranchWrapperEnum::FillImp<int8_t>)
      : (FillFn)(&I3ROOTBranchWrapperEnum::FillImp<uint8_t>);
  case 2:
    return type.is_signed ?
      (FillFn)(&I3ROOTBranchWrapperEnum::FillImp<int16_t>)
      : (FillFn)(&I3ROOTBranchWrapperEnum::FillImp<uint16_t>);
  case 4:
    return type.is_signed ?
      (FillFn)(&I3ROOTBranchWrapperEnum::FillImp<int32_t>)
      : (FillFn)(&I3ROOTBranchWrapperEnum::FillImp<uint32_t>);
  case 8:
    return type.is_signed ?
      (FillFn)(&I3ROOTBranchWrapperEnum::FillImp<int64_t>)
      : (FillFn)(&I3ROOTBranchWrapperEnum::FillImp<uint64_t>);

  default:
    log_fatal("Cannot book %zubit enums", type.size*8);
  }
}
