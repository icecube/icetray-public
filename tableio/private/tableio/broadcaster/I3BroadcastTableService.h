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

#ifndef I3BROADCASTTABLESERVICE_H_INCLUDED
#define I3BROADCASTTABLESERVICE_H_INCLUDED

#include <tableio/I3TableService.h>

class I3BroadcastTableService : public I3TableService {
    public:
        I3BroadcastTableService(const std::vector<I3TableServicePtr>& clients);
        void SetIndexConverter(I3ConverterPtr gen);
    protected:
        virtual I3TablePtr CreateTable(const std::string& tableName, 
                                       I3TableRowDescriptionConstPtr description);
        virtual void CloseFile();
    private:
        std::vector<I3TableServicePtr> clients_;
};

#endif /* end of include guard */
