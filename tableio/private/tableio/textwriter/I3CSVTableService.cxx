/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy$
 */

#include "tableio/textwriter/I3CSVTable.h"
#include "tableio/textwriter/I3CSVTableService.h"
#include "tableio/I3TableRowDescription.h"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

I3CSVTableService::I3CSVTableService(const std::string& foldername) : folderName_(foldername) {
    fs::remove_all( folderName_ );
    fs::create_directory( folderName_ );
    fs::create_directory( folderName_ + "/__I3Index__" );
}

I3TablePtr I3CSVTableService::CreateTable(const std::string& tableName, 
                               I3TableRowDescriptionConstPtr description) {
  I3TablePtr index_table;
  if (description->GetUseIndex()){
        I3TableRowDescriptionConstPtr index_desc = GetIndexDescription();
        std::string indexpath = folderName_ + "/__I3Index__"; 
        index_table = I3TablePtr(new I3CSVTable(*this,tableName,index_desc,indexpath));
  }
  return I3TablePtr(new I3CSVTable(*this,tableName,description,folderName_,index_table));
}

void I3CSVTableService::CloseFile() {
    
}

I3CSVTableService::~I3CSVTableService() {}

