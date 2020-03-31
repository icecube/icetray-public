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

#ifndef	I3_TABLEROWDESCRIPTION_H_INCLUDED
#define I3_TABLEROWDESCRIPTION_H_INCLUDED

// icetray includes
#include "icetray/I3Logging.h"
#include "icetray/I3PointerTypedefs.h"

#include "tableio/I3Datatype.h"

#include <string>
#include <vector>
#include <boost/unordered_map.hpp>

// boost::assign for less tedious enums
#include <boost/assign/list_inserter.hpp>
#include <boost/preprocessor/seq/transform.hpp>

#include "tableio/I3MemoryChunk.h"

/*****************************************************************************/

#define WRAP_ELEMENTS(R,DATA,ELEM) BOOST_PP_STRINGIZE(ELEM),DATA::ELEM
#define MAKE_ENUM_VECTOR(VECTORNAME,CLASS,ENUM_TYPE,FIELDS)		\
    std::vector< std::pair<std::string,CLASS::ENUM_TYPE> > VECTORNAME ;	\
    boost::assign::push_back(VECTORNAME) BOOST_PP_SEQ_TRANSFORM(WRAP_ELEMENTS,CLASS,FIELDS); \

/*****************************************************************************/

/*****************************************************************************/


class I3TableRowDescription {


public:
    I3TableRowDescription();
    virtual ~I3TableRowDescription();

    /* basic AddField */
    void AddField(const std::string& name, I3Datatype type,
		  const std::string& unit, const std::string& doc,
		  size_t arrayLength);

    /* convenience AddField */
    template<class T>
    void AddField(const std::string& name, 
		  const std::string& unit,
		  const std::string& doc, 
		  size_t arrayLength=1) 
    {
      AddField(name, I3DatatypeFromNativeType<T>(), 
		 unit, doc, arrayLength);
    }

    /* convenience AddEnumField - create hdf type and call AddField */
    template<typename enum_type>
    void AddEnumField(const std::string& name, 
		      const std::vector<std::pair<std::string,enum_type> > &elements,
		      const std::string& unit,
		      const std::string& doc,
		      size_t arrayLength=1) {
            
    I3Datatype enum_typus = I3DatatypeFromNativeType<enum_type>(elements);
    AddField(name, enum_typus, unit, doc, arrayLength);
    }

    bool CanBeFilledInto(boost::shared_ptr<const I3TableRowDescription> other) const;
        
    // getter and setter - remove them? no real encapsulation anyway
    const std::vector<std::string>& GetFieldNames() const;
    const std::vector<I3Datatype>&  GetFieldTypes() const;
    // units of bytes
    const std::vector<size_t>& GetFieldTypeSizes() const;
    const std::vector<size_t>& GetFieldByteOffsets() const;
    const std::vector<size_t>& GetFieldArrayLengths() const;
    const std::vector<std::string>& GetFieldUnits() const;
    const std::vector<std::string>& GetFieldDocStrings() const;

    size_t GetFieldColumn(const std::string& fieldName) const;
    size_t GetTotalByteSize() const;
    size_t GetTotalChunkSize() const;
    size_t GetNumberOfFields() const;

    // Will this description be used for an object flattened across multiple rows?
    bool isMultiRow_;
    bool GetIsMultiRow() const { return isMultiRow_; };
    void SetIsMultiRow(bool f) { isMultiRow_ = f; };

    bool GetUseIndex() const {return useIndex_;}
    void SetUseIndex(bool u) {useIndex_ = u;}

    bool GetUsePadding() const { return usePadding_; }
    void SetUsePadding(bool up) { usePadding_ = up; }  
  
    bool operator==(boost::shared_ptr<const I3TableRowDescription> other) const;
private:
    size_t GetNextOffset() const;
	
    std::vector<std::string> fieldNames_;
    // typedef std::map<std::string, size_t> fieldNameToIndex_t;
    typedef boost::unordered_map<std::string, size_t> fieldNameToIndex_t;
    fieldNameToIndex_t fieldNameToIndex_;
    std::vector<I3Datatype> fieldTypes_;
    std::vector<size_t> fieldTypeSizes_;
    std::vector<size_t> fieldArrayLengths_;
    std::vector<size_t> fieldByteOffsets_;
    std::vector<std::string> fieldUnits_;
    std::vector<std::string> fieldDocStrings_;
    bool useIndex_;
    bool usePadding_;

    friend I3TableRowDescription operator|(const I3TableRowDescription& lhs, const I3TableRowDescription& rhs);
    friend I3TableRowDescription& operator<<(I3TableRowDescription& lhs, const I3TableRowDescription& rhs);
    
    SET_LOGGER("I3TableRowDescription");
};
    
// declare template specialization for bools to let other file find
// the implementation in the .cxx file
template<> void I3TableRowDescription::AddField<bool>(const std::string& name,
                                                      const std::string& unit,
                                                      const std::string& doc,
                                                      size_t arrayLength);

I3_POINTER_TYPEDEFS( I3TableRowDescription );

#endif
