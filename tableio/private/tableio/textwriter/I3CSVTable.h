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

#ifndef	I3CSVTABLE_H_INCLUDED
#define I3CSVTABLE_H_INCLUDED

#include "tableio/I3Table.h"
#include "tableio/I3Datatype.h"
#include <fstream>

I3_FORWARD_DECLARATION(I3TableService);
I3_FORWARD_DECLARATION(I3TableRowDescription);
I3_FORWARD_DECLARATION(I3TableRow);

class I3CSVTable : public I3Table {
    public:
        I3CSVTable(I3TableService& service, const std::string& name,
                   I3TableRowDescriptionConstPtr description,
                   std::string& folderPath, I3TablePtr index = I3TablePtr());

        virtual ~I3CSVTable();

    protected:
        virtual void WriteRows(I3TableRowConstPtr row);
        void CreateTable();
        
    private:
        
        enum NativeType {
            UNKNOWN,
            NATIVE_SCHAR,
            NATIVE_SHORT,
            NATIVE_INT,
            NATIVE_LONG,
            NATIVE_LLONG,
            NATIVE_UCHAR,
            NATIVE_USHORT,
            NATIVE_UINT,
            NATIVE_ULONG,
            NATIVE_ULLONG,
            NATIVE_FLOAT,
            NATIVE_DOUBLE,
            NATIVE_BOOL
          };
        NativeType GetNativeType(const I3Datatype& dtype);
        std::vector<NativeType> fieldTypes_;
        
        template <typename T>
        void inline WriteField(T* value,size_t arraySize) {
            for (size_t i = 0; i < arraySize; i++) {
                WriteElement(&value[i]);
                if ((arraySize > 1) && (i != arraySize-1)) output_ << ",";
            }
        }
        
        template <typename T>
        void inline WriteElement(T* element) {
            output_ << *element;
        }
        
        std::string folderPath_;
        std::ofstream output_;

    SET_LOGGER("I3CSVTable");
};

/* Force chars to be written as digits */
template <>
void inline I3CSVTable::WriteElement(signed char* element) {
    output_ << static_cast<int>(*element);
};

template <>
void inline I3CSVTable::WriteElement(unsigned char* element) {
    output_ << static_cast<int>(*element);
};

#endif
