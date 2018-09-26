/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> Last changed by: $LastChangedBy$
 */

#ifndef	I3HDFTABLESERVICE_H_INCLUDED
#define I3HDFTABLESERVICE_H_INCLUDED

#include "tableio/I3TableService.h"
#include "dataio/I3FileStager.h"

#include <boost/make_shared.hpp>

// hdf5 includes
#include "H5Ipublic.h"

class I3HDFTableService : public I3TableService {
    public:
        I3HDFTableService(I3::dataio::shared_filehandle filename, int compress=1, char mode='w')
        {
            init(filename, compress, mode);
        }
        I3HDFTableService(const std::string& filename, int compress=1, char mode='w')
        {
            init(boost::make_shared<I3::dataio::filehandle>(filename), compress, mode);
        }
        I3HDFTableService(const std::string& filename, char mode)
        {
            init(boost::make_shared<I3::dataio::filehandle>(filename), 1, mode);
        }
        virtual ~I3HDFTableService();

    protected:
        virtual I3TablePtr CreateTable(const std::string& tableName, 
                                       I3TableRowDescriptionConstPtr description);
        virtual void CloseFile();

    private:
        void init(I3::dataio::shared_filehandle filename, int compress, char mode);
        void FindTables();

        hid_t fileId_;
        hid_t rootGroupId_;
        hid_t indexGroupId_;
        I3::dataio::shared_filehandle filename_;
        int compress_;
        bool fileOpen_;

    SET_LOGGER("I3HDFTableService");
};


#endif
