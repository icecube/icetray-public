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

#ifndef	I3CSVTABLESERVICE_H_INCLUDED
#define I3CSVTABLESERVICE_H_INCLUDED

#include "tableio/I3TableService.h"

class I3CSVTableService : public I3TableService {
    public:
        I3CSVTableService(const std::string& foldername);
        virtual ~I3CSVTableService();

    protected:
        virtual I3TablePtr CreateTable(const std::string& tableName, 
                                       I3TableRowDescriptionConstPtr description);
        virtual void CloseFile();

    private:

        std::string folderName_;

    SET_LOGGER("I3CSVTableService");
};


#endif
