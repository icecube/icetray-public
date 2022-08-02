.. 
.. copyright  (C) 2010
.. The Icecube Collaboration
.. 
.. $Id$
.. 
.. @version $Revision$
.. @date $LastChangedDate$
.. @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$

.. highlight:: c++


Making a new table writer service
=================================

There are writer services that produce HDF5 tables, ROOT files, and plain text
(CSV). To make a new writer service for your pet analysis format, you must
implement subclasses of :cpp:type:`I3Table` and :cpp:type:`I3TableService`.

I3Table
*******

.. cpp:type:: I3Table
    
   represents a table on disk
    
.. cpp:function:: void I3Table::WriteRows(I3TableRowConstPtr rows)
        
   Write the data in `rows` to disk
        
.. cpp:function:: void I3TableRow::Flush(size_t nrows)
        
   Flush nrows to disk.

Your subclass must implement :cpp:func:`WriteRows()`. If it implements any kind
of caching, you should also implement :cpp:func:`Flush()`. Here, for example, is
an implementation that writes comma-separated text (CSV).

WriteRows()
___________

::

    void I3CSVTable::WriteRows(I3TableRowConstPtr row) {
        unsigned int i,j;
        std::vector<NativeType>::iterator t_it;
        I3TableRowPtr rows = boost::const_pointer_cast<I3TableRow>(row);

        const std::vector<size_t>& arrayLengths = description_->GetFieldArrayLengths();
        std::vector<size_t>::const_iterator size_it;
        rows->SetEnumsAreInts(true);
        for (i = 0; i < rows->GetNumberOfRows(); i++) {
            rows->SetCurrentRow(i);
            for (t_it = fieldTypes_.begin(), size_it = arrayLengths.begin(), j = 0;
                 t_it != fieldTypes_.end();
                 t_it++, size_it++, j++) {
                switch (*t_it) {
                    case NATIVE_SCHAR:
                        WriteField(rows->GetPointer<signed char>(j),*size_it);
                        break;
                    case NATIVE_UCHAR:
                        WriteField(rows->GetPointer<unsigned char>(j),*size_it);
                        break;
                    case NATIVE_SHORT:
                        WriteField(rows->GetPointer<short>(j),*size_it);
                        break;
                    case NATIVE_USHORT:
                        WriteField(rows->GetPointer<unsigned short>(j),*size_it);
                        break;
                    case NATIVE_INT:
                        WriteField(rows->GetPointer<int>(j),*size_it);
                        break;
                    case NATIVE_UINT:
                        WriteField(rows->GetPointer<unsigned int>(j),*size_it);
                        break;
                    case NATIVE_LONG:
                        WriteField(rows->GetPointer<long>(j),*size_it);
                        break;
                    case NATIVE_ULONG:
                        WriteField(rows->GetPointer<unsigned long>(j),*size_it);
                        break;
                    case NATIVE_LLONG:
                        WriteField(rows->GetPointer<long long>(j),*size_it);
                        break;
                    case NATIVE_ULLONG:
                        WriteField(rows->GetPointer<unsigned long long>(j),*size_it);
                        break;
                    case NATIVE_FLOAT:
                        WriteField(rows->GetPointer<float>(j),*size_it);
                        break;
                    case NATIVE_DOUBLE:
                        WriteField(rows->GetPointer<double>(j),*size_it);
                        break;
                    case NATIVE_BOOL:
                        WriteField(rows->GetPointer<bool>(j),*size_it);
                        break;
                    default:
                        log_warn("Don't know how to write type '%s'",description_->GetFieldTypes().at(j).description.c_str());
                }
                if (t_it == fieldTypes_.end()-1) {
                    output_ << std::endl;
                } else {
                    output_ << ",";
                }
            }
        }
        rows->SetEnumsAreInts(false);
    }

For each field, a pointer to the `j` th field is cast to the appropriate native
type and passed to :cpp:func:`WriteField()`, which is just a templated function
that dumps the argument to :code:`iostream output_`::

    template <typename T>
    void WriteField(T* value,size_t arraySize) {
        for (size_t i = 0; i < arraySize; i++) {
            output_ << value[i];
            if ((arraySize > 1) && (i != arraySize-1)) output_ << ",";
        }
    }

Type-switching
______________

Since you don't have compile-time information about the types of the fields,
some type-switching is unavoidable. :cpp:type:`I3CSVTable` resolves and caches an
equivalent native type for each field::

    I3CSVTable::NativeType I3CSVTable::GetNativeType(const I3Datatype& dtype) {
        I3CSVTable::NativeType type = UNKNOWN;
        switch (dtype.kind) {
            case I3Datatype::Int:
                if (dtype.size == sizeof(char)) {
                    if (dtype.is_signed) type = NATIVE_SCHAR;
                    else                 type = NATIVE_UCHAR;
                } else if (dtype.size == sizeof(short)) {
                    if (dtype.is_signed) type = NATIVE_SHORT;
                    else                 type = NATIVE_USHORT;
                } else if (dtype.size == sizeof(int)) {
                    if (dtype.is_signed) type = NATIVE_INT;
                    else                 type = NATIVE_UINT;
                } else if (dtype.size == sizeof(long)) {
                    if (dtype.is_signed) type = NATIVE_LONG;
                    else                 type = NATIVE_ULONG;
                } else if (dtype.size == sizeof(long long)) {
                    if (dtype.is_signed) type = NATIVE_LLONG;
                    else                 type = NATIVE_ULLONG;
                }
                break;
            case I3Datatype::Float:
                if (dtype.size == sizeof(float)) {
                    type = NATIVE_FLOAT;
                } else if (dtype.size == sizeof(double)) {
                    type = NATIVE_DOUBLE;
                }
                break;
            case I3Datatype::Bool:
                type = NATIVE_BOOL;
                break;
            case I3Datatype::Enum:
                type = NATIVE_INT;
                break;
        }
        return type;
    }

Constructor
___________

You can set up any header information in your constructor. In
:cpp:type:`I3CSVTable`, :cpp:func:`CreateTable()` is called from the constructor::

    void I3CSVTable::CreateTable() {
        std::vector<std::string>::const_iterator name_it,unit_it,doc_it;
        std::vector<size_t>::const_iterator size_it;
        std::vector<I3Datatype>::const_iterator dtype_it;
        const std::vector<std::string>& names = description_->GetFieldNames();
        const std::vector<std::string>& units = description_->GetFieldUnits();
        const std::vector<std::string>& doc = description_->GetFieldDocStrings();
        const std::vector<size_t>& arrayLengths = description_->GetFieldArrayLengths();
        const std::vector<I3Datatype>& dtypes = description_->GetFieldTypes();
    
        for (name_it = names.begin(), unit_it = units.begin(), size_it = arrayLengths.begin(); 
             name_it != names.end(); 
             name_it++, unit_it++, size_it++) {
            output_ << "\"" << *name_it;
            if (unit_it->size() > 0) output_ << " [" << *unit_it << "]";
            output_ << "\"";
            if (*size_it > 1) {
                for (size_t i = 1; i < *size_it; i++) output_ << ",";
            }
            if (name_it == names.end()-1) {
                output_ << std::endl;
            } else {
                output_ << ",";
            }
        }
        for (doc_it = doc.begin(), size_it = arrayLengths.begin(), dtype_it = dtypes.begin();
             doc_it != doc.end();
             doc_it++, size_it++, dtype_it++) {
            output_ << "\"" << *doc_it;
            // if this is an enum type, write out the enum members
            if (dtype_it->kind == I3Datatype::Enum) {
                output_ << "{ ";
                std::vector<std::pair<std::string,long> >::const_iterator member_it;
                for (member_it = dtype_it->enum_members.begin(); 
                     member_it != dtype_it->enum_members.end(); 
                     member_it++) {
                    output_ << member_it->first << " = " << member_it->second;
                    if (member_it != dtype_it->enum_members.end()-1) output_ << ", ";
                }
                output_ << " }";
            }
            output_ << "\"";
            // if this is an array type, add enough columns for the array size
            if (*size_it > 1) {
                for (size_t i = 1; i < *size_it; i++) output_ << ",";
            }
            // terminate or continue the line
            if (doc_it == doc.end()-1) {
                output_ << std::endl;
            } else {
                output_ << ",";
            }
        }
    
        output_.precision(12);
        output_ << std::scientific;
    
        for (dtype_it = dtypes.begin(); dtype_it != dtypes.end(); dtype_it++) {
            fieldTypes_.push_back(GetNativeType(*dtype_it));
        }
    }

I3TableService
**************

To manage the collection of tables, you must implement a subclass of
I3TableService.

.. cpp:type:: I3TableService

    represents a bundle of tables on disk, possibly belonging to a single file
    
    .. cpp:function:: virtual I3TablePtr I3TableService::CreateTable(const std::string& tableName, I3TableRowDescriptionConstPtr description)
        
        Create a new table called `tableName` based on `description`.
        
    .. cpp:function:: virtual void I3TableService::CloseFile()
        
        Called at the end of operations.
            

CreateTable()
_____________

You must implement :cpp:func:`CreateTable()`, which is called whenever the writer
module requests a table that does not yet exist. The :cpp:type:`I3CSVTable`
implementation is fairly simple::

    I3TablePtr I3CSVTableService::CreateTable(const std::string& tableName, 
                                   I3TableRowDescriptionConstPtr description) {

            I3TableRowDescriptionConstPtr index_desc = GetIndexDescription();
            std::string indexpath = folderName_ + "/__I3Index__"; 
            I3TablePtr index_table(new I3CSVTable(*this,tableName,index_desc,indexpath));
            return I3TablePtr(new I3CSVTable(*this,tableName,description,folderName_,index_table));
    }

Two tables are created: one "data" table, and an auxiliary "index" table that
contains the starting and stopping positions of individual events. This is
nontrivial for compound objects like DOMLaunches that must be flattened across
multiple rows. The function returns a shared pointer to the data table.

CloseFile()
___________

:cpp:func:`CloseFile()` should perform any required clean-up, including flushing any internal caches.

Constructor
___________

You can implement any required setup in the constructor. :cpp:type:`I3CSVTable`,
for example, writes tables to individual files in a directory, so it
re-creates that directory in its constructor::

    I3CSVTableService::I3CSVTableService(const std::string& foldername) : folderName_(foldername) {
        fs::remove_all( folderName_ );
        fs::create_directory( folderName_ );
        fs::create_directory( folderName_ + "/__I3Index__" );
    }

Python bindings
_______________

You must also provide trivial Python bindings for your
:cpp:type:`I3TableService`::

    #include <icetray/load_project.h>
    #include "textwriter/I3CSVTableService.h"

    namespace bp = boost::python;

    I3_PYTHON_MODULE(textwriter)
    {
      load_project("textwriter", false);

      bp::class_<I3CSVTableService, 
                 boost::shared_ptr<I3CSVTableService>,
                 bp::bases<I3TableService> >
                 ("I3CSVTableService",
                  bp::init<const std::string>(bp::args("folder_name")))
                 ;
    }
    
