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

#include "tableio/I3TableRowDescription.h"
#include <numeric>

/******************************************************************************/

I3TableRowDescription::I3TableRowDescription() :
  isMultiRow_(false), useIndex_(true),usePadding_(true) {}

/******************************************************************************/

I3TableRowDescription::~I3TableRowDescription() {}

/******************************************************************************/

const std::vector<std::string>& I3TableRowDescription::GetFieldNames() const {
    return fieldNames_;
}

const std::vector<I3Datatype>&  I3TableRowDescription::GetFieldTypes() const {
    return fieldTypes_;
}

const std::vector<size_t>& I3TableRowDescription::GetFieldTypeSizes() const {
    return fieldTypeSizes_;
}

const std::vector<size_t>& I3TableRowDescription::GetFieldArrayLengths() const {
    return fieldArrayLengths_;
}

const std::vector<size_t>& I3TableRowDescription::GetFieldByteOffsets() const {
    return fieldByteOffsets_;
}

const std::vector<std::string>& I3TableRowDescription::GetFieldUnits() const {
    return fieldUnits_;
}

const std::vector<std::string>& I3TableRowDescription::GetFieldDocStrings() const {
    return fieldDocStrings_;
}

/******************************************************************************/
        
size_t I3TableRowDescription::GetFieldColumn(const std::string& fieldName) const {
    fieldNameToIndex_t::const_iterator iter;
    iter = fieldNameToIndex_.find(fieldName);
    if (iter != fieldNameToIndex_.end()) {
        return iter->second;
    }
    else {
        return GetNumberOfFields();
    }
}

/******************************************************************************/

bool I3TableRowDescription::CanBeFilledInto(I3TableRowDescriptionConstPtr other) const {
    size_t nfields = GetNumberOfFields();
    bool compatible = true;
    for (size_t i =0; i < nfields; ++i) {
        if ( (fieldNames_.at(i) != other->GetFieldNames().at(i) ) ||
             (fieldTypes_.at(i) != other->GetFieldTypes().at(i) ) ||
             (fieldTypeSizes_.at(i) != other->GetFieldTypeSizes().at(i) ) ) // redundant...
            compatible = false;
    }

    return compatible;
}

/******************************************************************************/

bool I3TableRowDescription::operator==(I3TableRowDescriptionConstPtr other) const {
   size_t nfields = GetNumberOfFields();
   size_t o_nfields = other->GetNumberOfFields();
   if (nfields != o_nfields) return false;
   if (isMultiRow_ != other->isMultiRow_) return false;
   bool equal = true;
   
   for (size_t i = 0; i < nfields; ++i) {
       if ( (fieldNames_.at(i) != other->GetFieldNames().at(i) ) ||
            (fieldTypes_.at(i) != other->GetFieldTypes().at(i) ) ||
            (fieldUnits_.at(i) != other->GetFieldUnits().at(i) ) ||
            (fieldDocStrings_.at(i) != other->GetFieldDocStrings().at(i) )          
          ) {
              equal = false;
              break;
            }
   }
   return equal;
}

/******************************************************************************/

    /* specialized AddField for booleans */
    template<>
    void I3TableRowDescription::AddField<bool>(const std::string& name, 
                  const std::string& unit,
                  const std::string& doc, 
                  size_t arrayLength) 
    {
                // Since booleans are just integers, we need to enforce this unit convention
                std::string boolunit("bool");
                if ((unit.size() != 0) && (unit != boolunit))
                        log_fatal("The unit string of a boolean field must be \"bool\".");
                AddField(name, I3DatatypeFromNativeType<bool>(), 
                         boolunit, doc, arrayLength);
    }


/******************************************************************************/

// Ensure that addresses are properly aligned. As long as the base
// address has a greater alignment than any of the types used in the
// description, it is sufficient to ensure that offsets are multiples
// of the type size.
static size_t
AlignOffset(size_t offset, size_t size)
{
	// We only align to powers of 2 in this house.
	size_t align = 1;
	while (align < size)
		align <<= 1;
	
	// If alredy aligned, hunky-dory. Otherwise, shift
	// offset up to next aligned address
	if (offset % align == 0)
		return offset;
	else
		return (offset + align) & ~(align-1);
}

/******************************************************************************/

void I3TableRowDescription::AddField(const std::string& name, I3Datatype type,
                                     const std::string& unit, const std::string& doc,
                                     size_t arrayLength) {

    size_t byteOffset=0;
    if (fieldByteOffsets_.size() > 0) {
        byteOffset = AlignOffset(GetNextOffset(), type.size);
    }    
    size_t nfields = fieldNameToIndex_.size();
    fieldNames_.push_back(name);
    fieldNameToIndex_[name] = nfields;

    // special case for ambigous integers that may be booleans
    if ( unit == std::string("bool") ) type.kind = I3Datatype::Bool;
    fieldTypes_.push_back(type);
    
    // check that the type actually fits in the memory chunk
    if (type.size > I3MEMORYCHUNK_SIZE)
        log_fatal("Type '%s' is larger than the memory chunk size!",type.description.c_str());

    fieldTypeSizes_.push_back(type.size);
    fieldArrayLengths_.push_back(arrayLength);
    fieldByteOffsets_.push_back(byteOffset);
    fieldUnits_.push_back(unit);
    fieldDocStrings_.push_back(doc);
}

/******************************************************************************/
        
size_t I3TableRowDescription::GetTotalChunkSize() const {
    return (GetNextOffset() + I3MEMORYCHUNK_SIZE - 1)/I3MEMORYCHUNK_SIZE;
}

size_t I3TableRowDescription::GetTotalByteSize() const {
    return I3MEMORYCHUNK_SIZE*GetTotalChunkSize();
}

size_t I3TableRowDescription::GetNextOffset() const {
    if (fieldByteOffsets_.size() == 0)
        return 0;
    else
        return fieldByteOffsets_.back() +
	    fieldTypeSizes_.back()*fieldArrayLengths_.back();
}



/******************************************************************************/
        
size_t I3TableRowDescription::GetNumberOfFields() const {
    return fieldNames_.size(); 
}

/******************************************************************************/

I3TableRowDescription& operator<<(I3TableRowDescription& lhs, 
                                const I3TableRowDescription& rhs) {
    size_t nfields_new = rhs.GetNumberOfFields();
    size_t nfields_old = lhs.GetNumberOfFields();
    for (size_t i = 0; i < nfields_new; i++) {
        const std::string &fieldName = rhs.fieldNames_.at(i);
	size_t typesize = rhs.fieldTypeSizes_.at(i);
	size_t byteOffset = 0;
        if (lhs.GetNumberOfFields() > 0)
          byteOffset = AlignOffset(lhs.GetNextOffset(), typesize);	

        lhs.isMultiRow_ = (lhs.isMultiRow_ || rhs.isMultiRow_);

        // values that are just copied:
        lhs.fieldNames_.push_back( fieldName );
        lhs.fieldTypeSizes_.push_back( typesize );
        lhs.fieldTypes_.push_back( rhs.fieldTypes_.at(i) );
        lhs.fieldArrayLengths_.push_back( rhs.fieldArrayLengths_.at(i) );
        lhs.fieldUnits_.push_back( rhs.fieldUnits_.at(i) );
        lhs.fieldDocStrings_.push_back( rhs.fieldDocStrings_.at(i) );

        // values that have to be recalculated:
        lhs.fieldNameToIndex_[fieldName] = nfields_old + i;
        lhs.fieldByteOffsets_.push_back(byteOffset);
    }
    return lhs;
}

I3TableRowDescription operator|(const I3TableRowDescription& lhs, 
                                const I3TableRowDescription& rhs) {
    I3TableRowDescription newlhs = I3TableRowDescription(lhs);
    newlhs << rhs;
    return newlhs;
}



