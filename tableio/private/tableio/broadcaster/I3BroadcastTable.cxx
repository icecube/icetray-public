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

#include <tableio/broadcaster/I3BroadcastTable.h>

I3BroadcastTable::I3BroadcastTable(I3TableService& service, std::string name,
        I3TableRowDescriptionConstPtr description, std::vector<I3TablePtr>& clients)
  : I3Table(service,name,description), clients_(clients)
{};

void I3BroadcastTable::WriteRows(I3TableRowConstPtr rows) {
    log_fatal("I should never have been called!");
};
 
void I3BroadcastTable::AddRow(I3EventHeaderConstPtr header, I3TableRowConstPtr row) {
    std::vector<I3TablePtr>::iterator iter;
    for(iter = clients_.begin(); iter != clients_.end(); ++iter ) {
        (*iter)->AddRow(header,row);
    }
};

void I3BroadcastTable::Align() {
    std::vector<I3TablePtr>::iterator iter;
    for(iter = clients_.begin(); iter != clients_.end(); ++iter ) {
        (*iter)->Align();
    }
};
