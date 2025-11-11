/*
 * Copyright  (C) 2010 The Icecube Collaboration
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * @author Fabian Kislat <fabian.kislat@desy.de> Last changed by: $LastChangedBy$
 */

#include "I3ROOTTableService.h"
#include "I3ROOTTable.h"
#include <tableio/I3TableRowDescription.h>

#include <TFile.h>
#include <TTree.h>

#include <boost/foreach.hpp>


namespace {

  struct InitTreeSize {
    InitTreeSize()
    {
      // set the maximum tree size to 1TB to essentially keep ROOT from
      // splitting files
      TTree::SetMaxTreeSize(1000000000000LL);
    }
  };

  static const InitTreeSize initTreeSize;

}

void
I3ROOTTableService::init(I3::dataio::shared_filehandle filename, const std::string &master,
				       int compress, const std::string &mode)
{

  // This is obscure, thus a slightly longer explanation
  // ===================================================
  // The following may seem like a memory leak, but it is not:
  // Any TTree created from now on will "live" inside this TFile, and will know
  // about it. If the file size reaches the size set with setMaxTreeSize, ROOT
  // will create a new file and close the existing one. All trees will know
  // about this change, but the pointer to this TFile object will become
  // invalid. Later on we can ask any TTree about the current file, obtaining a
  // valid TFile pointer.
  if (!(filename_ = filename))
    log_fatal("NULL file handle!");
  TFile *file = new TFile(filename->c_str(), mode.c_str(), "", compress);
  if (!file->IsOpen())
    log_fatal("Cannot open file %s", filename->c_str());

  mastertable_ = boost::dynamic_pointer_cast<I3ROOTTable>(GetTable(master, I3TableRowDescriptionPtr(new I3TableRowDescription)));
  open_ = true;
}

I3ROOTTableService::~I3ROOTTableService() {}

void I3ROOTTableService::setMaxTreeSize(long long int maxSize)
{
  TTree::SetMaxTreeSize(maxSize*1024*1024);
}

I3TablePtr I3ROOTTableService::CreateTable(const std::string &tableName,
					   I3TableRowDescriptionConstPtr description)
{
  // activate the right file
  if (mastertable_) mastertable_->tree_->GetCurrentFile()->cd();
  I3ROOTTablePtr table(new I3ROOTTable(*this, tableName, description));
  if (mastertable_) {  // will not do this for the master table
    tables_.push_back(table);
  }
  return table;
}

void I3ROOTTableService::CloseFile()
{
  if (open_) {
    mastertable_->Align();
    TFile *file = mastertable_->tree_->GetCurrentFile();
    file->cd();
    BOOST_FOREACH(I3ROOTTablePtr table, tables_) {
      mastertable_->tree_->AddFriend(table->tree_->GetName());
      table->Write();
    }
    mastertable_->Write();
    // close the file, one could ask any tree about the TFile pointer
    file->Close();
    open_ = false;
    filename_.reset();
  }
}
