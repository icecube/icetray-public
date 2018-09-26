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

#include <tableio/broadcaster/I3BroadcastTableService.h> 
#include <tableio/broadcaster/I3BroadcastTable.h>
#include <boost/foreach.hpp>

I3BroadcastTableService::I3BroadcastTableService(const std::vector<I3TableServicePtr>& clients) : I3TableService(), clients_(clients) {};


I3TablePtr I3BroadcastTableService::CreateTable(const std::string& tableName, 
                                   I3TableRowDescriptionConstPtr description) {
    std::vector<I3TableServicePtr>::iterator iter;
    std::vector<I3TablePtr> client_tables;
    for(iter = clients_.begin(); iter != clients_.end(); ++iter) {
        client_tables.push_back((*iter)->GetTable(tableName,description));
    }
    return I3TablePtr(new I3BroadcastTable(*this,tableName,description,client_tables));
                                       
};

void I3BroadcastTableService::SetIndexConverter(I3ConverterPtr gen)
{
	I3TableService::SetIndexConverter(gen);
	BOOST_FOREACH(I3TableServicePtr client, clients_)
		client->SetIndexConverter(gen);
}

void I3BroadcastTableService::CloseFile() {
    std::vector<I3TableServicePtr>::iterator iter;
    for(iter = clients_.begin(); iter != clients_.end(); ++iter) {
        (*iter)->Finish();
    }
};
