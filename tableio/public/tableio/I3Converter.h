/*
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> Last changed by: $LastChangedBy$
 */

#ifndef	I3CONVERTER_H_INCLUDED
#define I3CONVERTER_H_INCLUDED

#include <string>
#include <vector>
#include <utility>

#include "icetray/IcetrayFwd.h"
#include "icetray/I3FrameObject.h"
#include "icetray/I3Frame.h"

#include "tableio/I3TableRowDescription.h"
#include "tableio/I3TableRow.h"

I3_FORWARD_DECLARATION(I3TableWriter);

/******************************************************************************/

/**
 * \class I3Converter
 * \brief The basic interface of a tableio converter.
 *
 * The basic interface to the individual converters. For this class pointer typdefs 
 * will be created. All methods take unspecialized pointers to I3FrameObjects.
 * While this is the converter interface used by tableio, custom converters should
 * instead be derived from I3ConverterImplementation.
 */
class I3Converter {
    public:
        virtual ~I3Converter();

        /**
	 * \brief Determine the number of rows the would be produced when converting
	 * the given object.
	 *
         * Allows to show an I3FrameObject to this converter to get the number
         * of rows that this converter will produce
         */
        virtual size_t GetNumberOfRows(I3FrameObjectConstPtr object)=0;

        /**
	 * \brief Determine the number of rows the would be produced when converting
	 * the given object.
	 *
         * Allows to show an I3FrameObject to this converter to get the number
         * of rows that this converter will produce
         */
        virtual size_t GetNumberOfRows(const I3FrameObject& object)=0;

        /**
	 * \brief Return an I3TableRowDescription specifying the columns this converter
	 * will fill.
	 */
        virtual I3TableRowDescriptionConstPtr GetDescription(I3FrameObjectConstPtr object) = 0;
        /**
	 * \brief Return an I3TableRowDescription specifying the columns this converter
	 * will fill.
	 *
	 * An overload of the function above for const I3FrameObject references.
	 */
        virtual I3TableRowDescriptionConstPtr GetDescription(const I3FrameObject& object) = 0;

        /**
	 * \brief Return a pointer to an already created description object.
	 *
	 * This function will log_fatal if no description exists. Check for existance
	 * of the description with HasDescription().
         */
        I3TableRowDescriptionConstPtr GetDescription();

        /// Check if a description object has already been created
        bool HasDescription() { if (description_) return true; else return false; };
        
        /**
	 * \brief Fill an object into the table.
	 *
         * Fills the object held by pointer object into rows. To allow more
	 * complex converters a pointer to the current frame can be passed along.
	 * \param object The frame object to be converted
	 * \param rows The table rows to be filled
	 * \param frame Optional pointer to the current frame, allows direct access
	 *              to all frame contents.
         * \returns The number of rows it wrote
         */
        virtual size_t Convert(I3FrameObjectConstPtr object, 
                                     I3TableRowPtr rows, 
                                     I3FramePtr frame=I3FramePtr()) = 0;
        
        /**
	 * \brief Fill an object into the table.
	 *
	 * An overload of the function above for const I3FrameObject references.
	 */
        virtual size_t Convert(const I3FrameObject& object, 
                                     I3TableRowPtr rows, 
                                     I3FramePtr frame=I3FramePtr()) = 0;

        enum ConvertState{
            /// Used to indicate that a converter cannot convert a given object
            NoConversion,
            /// Used to indicate that a given object is exactly the type
            /// converted by a converter
            ExactConversion,
            /// Used to indicate that a given object is a type derived from the
            /// one a converter is designed to convert
            InexactConversion
        };
    
        /**
         * \brief Check if the converter can handle the given object.
         *
         * \returns A value indicating whether the converter can handle the 
         *          object, and if so whether it can do so specifically or 
         *          more generically.
         */
        virtual ConvertState CanConvert(I3FrameObjectConstPtr object) = 0;

        virtual I3Frame::Stream GetStop() = 0;
        
    protected:
        /// Pointer to the table row description
        I3TableRowDescriptionPtr description_;
        /**
	 * \brief Pointer to the current frame.
	 *
	 * Implementations of more complex converters can use this to access
	 * all contents of the frame, instead of just a single frame object.
	 */
        I3FramePtr currentFrame_;

};

I3_POINTER_TYPEDEFS( I3Converter );

/******************************************************************************/

/**
 * \class I3ConverterImplementation
 * \brief The base class for the individual converter implementations
 *
 * The pointer conversion from I3FrameObject to the respective type of the
 * converter is handled here. Also it somewhat hides the passed I3FramePtr.
 * So the majority of the converters can follow the scheme of converting
 * only a single object and more complex converters can be build that
 * access <code>currentFrame_</code> to get what they need. 
 */
template<class FrmObj>
class I3ConverterImplementation : public I3Converter {
    public:
        
        typedef FrmObj booked_type;

        virtual ~I3ConverterImplementation() { }
        
        /**
	 * \brief Determine the number of rows the would be produced when converting
	 * the given object.
	 *
         * This function handles the conversion to the specific frame object type
	 * and calls the overloaded protected function, which can be overloaded by
	 * derived converters.
         */
        size_t GetNumberOfRows(I3FrameObjectConstPtr object) {
            return GetNumberOfRows(dynamic_cast<const FrmObj&>(*object));
        }
        
        /**
	 * \brief Determine the number of rows the would be produced when converting
	 * the given object.
	 *
         * This function handles the conversion to the specific frame object type
	 * and calls the overloaded protected function, which can be overloaded by
	 * derived converters.
         */
        size_t GetNumberOfRows(const I3FrameObject& object) {
            return GetNumberOfRows(dynamic_cast<const FrmObj&>(object));
        }
        
        /**
	 * \brief Fills a frame object into the given table rows.
	 *
	 * This function calls FillRows which has to be implemented in the derived
	 * class. It handles the conversion of the generic I3FrameObject to the 
	 * specific class FrmObj given as the template parameter. Also the lazy
	 * initialization of the description is handled here: A table row
	 * description will only be created when the first object is converted,
	 * thus allowing the converter to adjust the description to the specific
	 * object. This is useful, e.g., for constant size
	 * <code>map&lt;string, object&gt;</code> type lists where each entry will
	 * generate one column in the table.
	 * \param object The frame object to be converted
	 * \param rows The table rows to be filled
	 * \param frame Optional pointer to the current frame, allows direct access
	 *              to all frame contents.
         * \returns The number of rows it wrote
         */
        size_t Convert(const I3FrameObject& object, I3TableRowPtr rows, 
                             I3FramePtr frame) {
            const FrmObj& typedObject = dynamic_cast<const FrmObj&>(object);

            // lazy initialization of the description:
            if (!description_)
               description_ = CreateDescription(typedObject);

            currentFrame_ = frame;

            size_t nrows = FillRows( typedObject, rows );
            currentFrame_.reset();
            return nrows;
        }
        
        /**
	 * \brief Fills a frame object into the given table rows.
	 *
	 * An overload of the function above.
	 */
        size_t Convert(I3FrameObjectConstPtr object, I3TableRowPtr rows, I3FramePtr frame) {
            return Convert( *object, rows, frame);
        }

        /**
	 * \brief Return an I3TableRowDescription specifying the columns this converter
	 * will fill.
	 *
	 * The description will be re-created every time this method is called.
	 */
        I3TableRowDescriptionConstPtr GetDescription(const I3FrameObject& object) {
            description_ = CreateDescription(dynamic_cast<const FrmObj&>(object)); 
            return description_;
        }

        /**
	 * \brief Return an I3TableRowDescription specifying the columns this converter
	 * will fill.
	 *
	 * An overload of the function above.
	 */
        I3TableRowDescriptionConstPtr GetDescription(I3FrameObjectConstPtr object) {
            return GetDescription(*object);
        }

        /// Check if the converter is able to treat the given frame object.
        virtual ConvertState CanConvert(I3FrameObjectConstPtr object);

        I3Frame::Stream GetStop(){
          return I3Frame::Physics;
        }

    protected:
        /**
	 * \brief Create a table row description based on the given object.
	 *
	 * This function has to be implemented by the derived converter implementation.
	 */
        virtual I3TableRowDescriptionPtr CreateDescription(const FrmObj& object) = 0;
        /**
	 * \brief Returns the number of rows the converter would fill for the given
	 * frame object.
	 *
	 * The default implementation always returns 1. Override in the derived
	 * converter implementation if a different number of rows might be created.
	 */
        virtual size_t GetNumberOfRows(const FrmObj& object);
        /**
	 * \brief Fill a table row with the data from the given object.
	 *
	 * This function has to be implemented by the derived converter implementation.
	 */
        virtual size_t FillRows(const FrmObj& object, I3TableRowPtr rows) = 0;
};

/******************************************************************************/

// provide a default implemenation for GetNumberOfRows. covers all one-row-per-object converters.
template <class FrmObj>
size_t I3ConverterImplementation<FrmObj>::GetNumberOfRows(const FrmObj& object) {
    return 1;
}

/******************************************************************************/

/// A default implementation of CanConvert. Tests whether the pointer matches the
/// template type, and if that fails tests casting.
template <class FrmObj>
I3Converter::ConvertState
I3ConverterImplementation<FrmObj>::CanConvert(I3FrameObjectConstPtr object) {
    const I3FrameObject& obj=*object.get();
    if(typeid(FrmObj) == typeid(obj))
        return(ExactConversion);
    if(dynamic_cast<const FrmObj*>(&obj) != NULL)
        return(InexactConversion);
    return(NoConversion);
}

/******************************************************************************/

// The unsigned integer type to use when writing size_t out
// to a table. This should be the largest pointer size of any platform
// on which IceTray runs (64 bits for the forseeable future).
typedef uint64_t tableio_size_t;

#endif
