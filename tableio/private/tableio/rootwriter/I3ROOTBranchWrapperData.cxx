/*
 * Copyright  (C) 2010 The Icecube Collaboration
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * @author Fabian Kislat <fabian.kislat@desy.de> Last changed by: $LastChangedBy$
 */

#include "I3ROOTBranchWrapperData.h"

#include <icetray/I3Logging.h>
#include <tableio/I3Datatype.h>
#include <tableio/I3TableRow.h>

#include <TBranch.h>
#include <TTree.h>

#include <boost/lexical_cast.hpp>

#include <cassert>
#include <cctype>
#include <cstring>


char I3DatatypeToROOTType(const I3Datatype &type)
{
  char typechar = '\0';

  switch (type.kind) {
  case I3Datatype::Float:
    if (type.size == 4) {
      typechar = 'F';
    } else if (type.size == 8) {
      typechar = 'D';
    } else {
      log_fatal("Cannot book variables of type '%s' with a size of %zu bits",
		type.description.c_str(), type.size*8);
    }
    break;
  case I3Datatype::Int:
  case I3Datatype::Enum:
    if (type.size == 1) {
      typechar = 'b';
    } else if (type.size == 2) {
      typechar = 's';
    } else if (type.size == 4) {
      typechar = 'i';
    } else if (type.size == 8) {
      typechar = 'l';
    } else {
      log_fatal("Cannot book variables of type %s with a size of %zu bits",
		type.description.c_str(), type.size*8);
    }
    break;
  case I3Datatype::Bool:
    typechar = 'O';
    break;
  default:
    assert(false);
  }

  if (type.is_signed) typechar = toupper(typechar);

  return typechar;
}


I3ROOTBranchWrapperData::I3ROOTBranchWrapperData()
  : I3ROOTBranchWrapper(), datasize_(0), data_(new std::vector<char>)
{}

I3ROOTBranchWrapperData::I3ROOTBranchWrapperData(TTree *tree, const I3Datatype &type,
						 const std::string &branchname,
						 const std::string &docstring,
						 unsigned int index,
						 size_t arrayLength,
						 const I3ROOTBranchWrapperConstPtr &counter)
  : I3ROOTBranchWrapper(tree, index, arrayLength, (bool)counter),
    datasize_(type.size), data_(new std::vector<char>),
    rootCharArrayHack_(false), rootCharArrayIsSigned_(true)
{
  // generate a type description
  std::string leafdescription = branchname;

  // create the field to hold the data
  data_->resize(datasize_*arrayLength_);

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

  char typechar = I3DatatypeToROOTType(type);

  // when reading the tree (py)ROOT cannot distinguish an array of (un)signed char
  // and a string (char*)
  // workaround: book array of signed shorts
  // a single (un)signed byte should be fine
  if ((arrayLength_ > 1) || multirow_) {
    // this requires some conversion when filling the data (see below)
    // if anyone comes up with a better idea (or fixes ROOT), I'd be VERY happy to hear about it
    if (typechar == 'B') {
      typechar = 'S';
      rootCharArrayHack_ = true;
      rootCharArrayIsSigned_ = true;
    } else if (typechar == 'b') {
      typechar = 's';
      rootCharArrayHack_ = true;
      rootCharArrayIsSigned_ = false;
    }
  }

  // generate the type of the field
  leafdescription.push_back('/');
  leafdescription.push_back(typechar);

  // finally: create the branch
  TBranch *branch = tree->Branch(branchname.c_str(), &(data_->at(0)),
				  leafdescription.c_str());
  branch->SetTitle(docstring.c_str());
  SetBranch(branch);
}

I3ROOTBranchWrapperData::I3ROOTBranchWrapperData(const I3ROOTBranchWrapperData &rhs)
  : I3ROOTBranchWrapper(rhs), data_(rhs.data_)
{}

I3ROOTBranchWrapperData::~I3ROOTBranchWrapperData() {}

void I3ROOTBranchWrapperData::Fill(const I3TableRowConstPtr &data)
{
  // in the particular case that we are booking an array of signed bytes
  // some conversion needs to be done (see constructor)
  if (rootCharArrayHack_) {
    if (rootCharArrayIsSigned_)
      RootCharArrayHackFillData<int8_t, int16_t>(data);
    else
      RootCharArrayHackFillData<uint8_t, uint16_t>(data);
    return;
  }

  // make sure we have enough room to store all elements
  if (data->GetNumberOfRows()*datasize_*arrayLength_ > data_->size()) {
    char *olddata = &(data_->at(0));
    data_->resize(data->GetNumberOfRows()*datasize_*arrayLength_);
    if (olddata != &(data_->at(0))) {
      setBranchAddress();
    }
  }

  // copy all elements to our internal data array
  size_t fieldlength = datasize_*arrayLength_;
  for (unsigned int row = 0; row < data->GetNumberOfRows(); ++row) {
    const void *source = data->GetPointerToField(index_, row);
    memcpy(&(data_->at(row*fieldlength)), source, fieldlength);
  }
}

void I3ROOTBranchWrapperData::setBranchAddress()
{
  branch_->SetAddress(&(data_->at(0)));
}

template <typename Source, typename Destination>
void I3ROOTBranchWrapperData::RootCharArrayHackFillData(const I3TableRowConstPtr &data)
{
  // this function really is just a hack to allow for the explicit
  // conversion of char arrays to short arrays
  // (see explanation in the constructor)

  static const size_t datasize = sizeof(Destination);

  // make sure we have enough room to store all elements
  if (data->GetNumberOfRows()*datasize*arrayLength_ > data_->size()) {
    char *olddata = &(data_->at(0));
    data_->resize(data->GetNumberOfRows()*datasize*arrayLength_);
    if (olddata != &(data_->at(0))) {
      setBranchAddress();
    }
  }

  // copy all elements to our internal data array and convert them to int16_t on the way
  size_t fieldlength = datasize*arrayLength_;
  for (unsigned int row = 0; row < data->GetNumberOfRows(); ++row) {
    const Source *source = data->GetPointer<Source>(index_, row);
    Destination *localdata = reinterpret_cast<Destination*>(&data_->at(row*fieldlength));
    for (size_t i = 0; i < arrayLength_; ++i) {
      localdata[i] = source[i];
    }
  }
}
