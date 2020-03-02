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

#ifndef	I3TABLEWRITER_H_INCLUDED
#define I3TABLEWRITER_H_INCLUDED

#include "icetray/IcetrayFwd.h"

#include <string>
#include <set>

#include "tableio/I3Converter.h"
#include "tableio/detail/I3ConverterMill.h"
#include <boost/python.hpp>

I3_FORWARD_DECLARATION(I3TableService);
I3_FORWARD_DECLARATION(I3Table);

class I3TableWriter {
    public:
        I3TableWriter(I3TableServicePtr service, std::vector<I3ConverterMillPtr>& converters, std::vector<std::string>& streams);
        virtual ~I3TableWriter();
        
        // register one specific object, lazily. if type and converter are empty the writer 
        // should figure out appropriate values
        bool AddObject(std::string name, std::string tableName, I3Frame::Stream frame_stop,
                       I3ConverterPtr converter, I3FrameObjectConstPtr obj);
        
        struct TableSpec {
           std::string tableName;
           I3ConverterPtr converter;
           TableSpec(const std::string name,I3ConverterPtr conv) : tableName(name),converter(conv) {};
           TableSpec(const std::string name) : tableName(name),converter() {};
           TableSpec(I3ConverterPtr conv) : tableName(),converter(conv) {};
           TableSpec() : tableName(),converter() {};
        };
        
        struct TypeSpec {
           boost::python::object held_type_;
           
           bool operator<(const TypeSpec& rhs) const {
              return( held_type_.ptr() < rhs.held_type_.ptr() );
           }
        
           TypeSpec(boost::python::object t_) : held_type_(t_) { }
        
           bool check(I3FrameObjectConstPtr p) const {
              return check(boost::const_pointer_cast<I3FrameObject>(p));
           }
           
           bool check(I3FrameObjectPtr p) const {
              boost::python::object frameobj(p);
#ifndef I3_COMPILE_OUT_VERBOSE_LOGGING
              std::string cls   = boost::python::extract<const std::string>(frameobj.attr("__class__").attr("__name__"));
              std::string cname = boost::python::extract<const std::string>(held_type_.attr("__name__"));
              log_trace("Checking object of type '%s' against '%s'",
                       cls.c_str(),
                       cname.c_str());
#endif
              int rv = PyObject_IsInstance(frameobj.ptr(),held_type_.ptr());
              return (rv > 0);
           }
        };
      
        // add object to wanted list
        void AddObject(std::string typeName, TableSpec spec);
        // write all objects with this type
        void AddType(TypeSpec type, TableSpec spec);
        
        void AddConverter(std::string typeName, I3ConverterPtr converter);

        void Setup();

        void Convert(I3FramePtr frame);
        
        void Finish();




    protected:
        I3TablePtr ConnectTable(std::string tableName, 
                                const I3TableRowDescription& description);
        void DisconnectTable(I3TablePtr& table);
        
        
        // group components that belong together
        struct TableBundle {
            I3ConverterPtr converter;
            I3TablePtr table;
        };


        I3TableServicePtr service_;
        std::map<std::string, std::vector<TableBundle> > tables_;
        std::map<std::string, I3ConverterPtr> converters_;
        std::vector<I3ConverterMillPtr> converterCache_;
        std::vector<std::string> streams_;
        std::set<std::string> ignoredStreams_;
        // keys that have been examined and found useless
        std::set<std::string> uselessKeys_;
         
        typedef std::map<std::string, std::vector<TableSpec> > tablespec_map;
        typedef std::map<TypeSpec, std::vector<TableSpec> > typespec_map;
        // configuration lists and maps
        tablespec_map wantedNames_;
        typespec_map wantedTypes_; 
      
        std::map<std::string,std::string> objNameToTableName_;
        std::map<std::string,std::string> typeNameToConverterName_;
        I3FrameConstPtr currentFrame_;
        I3ConverterPtr ticConverter_;

    private:
        I3TableWriter();
        I3TableWriter(const I3TableWriter& rhs);

        const std::string GetTypeName(I3FramePtr, const std::string&);

        I3ConverterPtr FindConverter(I3FrameObjectConstPtr obj);

        friend struct I3TableWriterTestAccess;

    SET_LOGGER("I3TableWriter");
};

I3_POINTER_TYPEDEFS( I3TableWriter );

#endif
