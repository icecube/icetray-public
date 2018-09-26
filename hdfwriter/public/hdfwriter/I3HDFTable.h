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

#ifndef	I3HDFTABLE_H_INCLUDED
#define I3HDFTABLE_H_INCLUDED

#include "tableio/I3Table.h"
#include "tableio/I3Datatype.h"

#include "H5Ipublic.h"

I3_FORWARD_DECLARATION(I3TableService);
I3_FORWARD_DECLARATION(I3TableRowDescription);
I3_FORWARD_DECLARATION(I3TableRow);

class I3HDFTable : public I3Table {
    public:
        I3HDFTable(I3TableService& service, const std::string& name,
                   I3TableRowDescriptionConstPtr description,
                   hid_t fileid, int compress, I3TablePtr index = I3TablePtr());

        I3HDFTable(I3TableService& service, const std::string& name,
                   hid_t fileId, I3TablePtr index = I3TablePtr());
        virtual ~I3HDFTable();
        static std::string ReadAttributeString(hid_t fileID, std::string& where, std::string attribute);
        static hid_t GetHDFType(const I3Datatype& dtype, const size_t arrayLength);
        static I3Datatype GetI3Datatype(hid_t dtype, size_t* arrayLength);
        
        virtual I3TableRowConstPtr ReadRows(size_t start, size_t nrows) const;
        
        virtual void Flush(const size_t nrows = 0);

    protected:
        virtual void WriteRows(I3TableRowConstPtr row);
        virtual std::pair<size_t,size_t> GetRangeForEvent(size_t index) const;
        void CreateTable(int& compress);
        void CreateDescription();
        hid_t fileId_;
        
    private:
        I3TableRowPtr writeCache_;
        mutable I3TableRowPtr readCache_;
        mutable std::pair<size_t,size_t> readCacheExtent_;
        size_t chunkSize_;
        void CreateCache();
        void CalculateChunkSize();
        I3TableRowPtr ReadRowsFromTable(size_t start, size_t nrows) const;
        std::string log_label();


    SET_LOGGER("I3HDFTable");
};

// Data will be written in chunks of this size (64 kB)
// see http://www.pytables.org/docs/manual/ch05.html
#define CHUNKSIZE_BYTES 65535
// Buffer this many chunks before writing
#define CHUNKTIMES 2
#endif
