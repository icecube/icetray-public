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

#ifndef	I3TABLE_H_INCLUDED
#define I3TABLE_H_INCLUDED

#include <string>

#include "icetray/IcetrayFwd.h"
//#include "tableio/I3TableService.h"

I3_FORWARD_DECLARATION(I3TableRowDescription);
I3_FORWARD_DECLARATION(I3TableRow);
I3_FORWARD_DECLARATION(I3EventHeader);
I3_FORWARD_DECLARATION(I3TableService);

/**
 * \brief Base class for tableio tables.
 */
class I3Table {
    public:
        I3Table(I3TableService& service, std::string name,
                I3TableRowDescriptionConstPtr description);
        
        virtual ~I3Table(); // Flush?
        virtual void Flush(const size_t nrows = 0);
        
        bool IsConnectedToWriter();
        void SetConnectedToWriter(bool connected);

        I3TableRowPtr CreateRow(size_t nrows);
        virtual void AddRow(I3EventHeaderConstPtr header, I3TableRowConstPtr row);
        virtual void Align();
        
        // I3TableRowConstPtr GetRowForEvent(unsigned int RunID, unsigned int EventID);
        I3TableRowConstPtr GetRowForEvent(size_t index) const;

        std::string GetName() const; 
        size_t GetNumberOfEvents() const; 
        size_t GetNumberOfRows() const; 
        I3TableRowDescriptionConstPtr GetDescription();

    protected:
        // to be overridden by derivatives
        virtual void WriteRows(I3TableRowConstPtr row) = 0;
        virtual I3TableRowConstPtr ReadRows(size_t start, size_t nrows) const;
        virtual std::pair<size_t,size_t> GetRangeForEvent(size_t index) const;
        bool DoPadding();

        I3TableService& service_;
        std::string name_;

        I3TableRowDescriptionConstPtr description_;
        boost::shared_ptr<I3Table> indexTable_;

        size_t nevents_; // number of events written
        size_t nrows_;   // number of rows (>= events) written
        size_t nrowsWithPadding_;   // number of rows (>= events) written, including padding
        bool connected_;       // connected to a writer
        bool tableCreated_;    // the table/tree has been created successfully

        I3EventHeaderConstPtr lastHeader_;
        
        enum AlignmentType {
            MultiRow,   // Some objects can span multiple rows
            Strict      // All objects are written to exactly one row
        };
        
        virtual AlignmentType GetAlignmentType();

    private:
        I3Table();

    SET_LOGGER("I3Table");

};

I3_POINTER_TYPEDEFS( I3Table );



#endif
