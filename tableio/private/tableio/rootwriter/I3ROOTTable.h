/**
 * Copyright  (C) 2010 The Icecube Collaboration
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * @author Fabian Kislat <fabian.kislat@desy.de> Last changed by: $LastChangedBy$
 */

#ifndef	I3ROOTTABLE_H_INCLUDED
#define I3ROOTTABLE_H_INCLUDED

#include "icetray/I3Logging.h"
#include <tableio/I3Table.h>

#include <vector>


I3_FORWARD_DECLARATION(I3TableService);
I3_FORWARD_DECLARATION(I3TableRowDescription);
I3_FORWARD_DECLARATION(I3TableRow);
I3_FORWARD_DECLARATION(I3ROOTBranchWrapper);
I3_FORWARD_DECLARATION(I3ROOTBranchWrapperData);

class TBranch;
class TTree;
class I3ROOTTableService;


class I3ROOTTable : public I3Table {
public:
  I3ROOTTable(I3TableService& service, const std::string& name,
	      I3TableRowDescriptionConstPtr description);
  virtual ~I3ROOTTable();

  void Write();

protected:
  virtual void WriteRows(I3TableRowConstPtr row);
  virtual I3Table::AlignmentType GetAlignmentType();

private:
  TTree *tree_;
  bool multirow_;
  I3ROOTBranchWrapperDataPtr counter_;
  std::vector<I3ROOTBranchWrapperPtr> branches_;

  friend class I3ROOTTableService;

  SET_LOGGER("I3ROOTTable");
};

I3_POINTER_TYPEDEFS( I3ROOTTable );

#endif
