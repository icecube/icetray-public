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

#include <tableio/I3TableService.h>
#include <tableio/I3Table.h>
#include <tableio/converter/I3IndexColumnsGenerator.h>

#ifndef	I3TABLETRANSCRIBER_H_INCLUDED
#define I3TABLETRANSCRIBER_H_INCLUDED

class I3TableTranscriber {
    public:
        I3TableTranscriber(I3TableServicePtr input, I3TableServicePtr output);
        void Execute();
        void Execute(size_t nframes);
        
        void Finish();
        
    protected:
         I3TablePtr ConnectTable(std::string tableName, 
                                 const I3TableRowDescription& description);
         void DisconnectTable(I3TablePtr& table);
    private:
 
        I3TableTranscriber();
        I3TableServicePtr inputService_;
        I3TableServicePtr outputService_;
        // std::map<std::string, I3TablePtr> inputTables_;
	boost::shared_ptr<I3IndexColumnsGenerator> indexer_;
        std::vector<std::pair<I3TablePtr,I3TablePtr> > transcriptions_;
        size_t nEvents_;
    SET_LOGGER("I3TableTranscriber");
};

#endif

