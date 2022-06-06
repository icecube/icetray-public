/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$
 */
#include <string>

#include "tableio/converter/pybindings.h"
#include "tableio/converter/dataclasses_map_converters.h"
#include "tableio/converter/dataclasses_vector_converters.h"
#include "tableio/converter/I3DirectionConverter.h"
#include "tableio/converter/I3MapStringConverter.h"
#include "tableio/converter/I3MapStringVectorDoubleConverter.h"
#include "tableio/converter/I3ParticleConverter.h"
#include "tableio/converter/I3PositionConverter.h"
#include "tableio/converter/I3TreeConverter.h"
#include "dataclasses/physics/I3MCTree.h"
#include "tableio/converter/I3WaveformConverter.h"
#include "tableio/converter/PODConverter.h"
#include "tableio/converter/I3EventHeaderConverter.h"
#include "tableio/converter/I3WaveformSeriesMapConverter.h"
#include "tableio/converter/I3FilterResultMapConverter.h"

#include "dataclasses/I3MapOMKeyMask.h"

namespace {

  static boost::shared_ptr<I3WaveformSeriesMapConverter> makeWaveformSeriesMapConverter(bool bookGeometry)
  {
    return boost::shared_ptr<I3WaveformSeriesMapConverter>(new I3WaveformSeriesMapConverter(false, bookGeometry));
  }

}

// Make the RecoPulseSeriesMapConverter work on pulse masks.
class I3RecoPulseSeriesMapMaskConverter : public I3ConverterImplementation<I3RecoPulseSeriesMapMask>
{
public:
    // Again here, different constructors for different argument combinations (both, neither, one, the other)
    I3RecoPulseSeriesMapMaskConverter() : base_() {};
    I3RecoPulseSeriesMapMaskConverter(bool b, std::string btp) : base_(b, btp) {};
    I3RecoPulseSeriesMapMaskConverter(bool b) : base_(b) {};
    I3RecoPulseSeriesMapMaskConverter(std::string btp) : base_(btp) {};
    I3TableRowDescriptionPtr CreateDescription(const I3RecoPulseSeriesMapMask& m)
    {
        I3RecoPulseSeriesMap mappy;
        return boost::const_pointer_cast<I3TableRowDescription>(base_.GetDescription(mappy));
    }
    size_t GetNumberOfRows(const I3RecoPulseSeriesMapMask &mask)
    {
        return mask.GetSum();
    }
    size_t FillRows(const I3RecoPulseSeriesMapMask &mask, I3TableRowPtr rows)
    {
        I3RecoPulseSeriesMapConstPtr pulses = mask.Apply(*currentFrame_);
        return base_.Convert(*pulses, rows, currentFrame_);
    }
private:
    typedef I3MapOMKeyVectorConverter<convert::I3RecoPulse, I3RecoPulseSeriesMap> Base;
    Base base_;
};

void register_dataclasses_converters() {
    I3CONVERTER_NAMESPACE(dataclasses);

    // typdef the template into a legal Python identifier
    I3CONVERTER_EXPORT_DEFAULT(I3EventHeaderConverter,"Dumps I3EventHeader objects");

    typedef I3MapOMKeyVectorConverter< convert::I3DOMLaunch > I3DOMLaunchSeriesMapConverter;
    I3_MAP_CONVERTER_EXPORT_DEFAULT(I3DOMLaunchSeriesMapConverter,"Dumps all DOMLaunches verbatim.");
    typedef I3MapOMKeyVectorConverter< convert::I3RecoPulse > I3RecoPulseSeriesMapConverter;
    I3_MAP_CONVERTER_EXPORT_DEFAULT(I3RecoPulseSeriesMapConverter,"Dumps all RecoPulses verbatim.");
    I3_MAP_CONVERTER_EXPORT_DEFAULT(I3RecoPulseSeriesMapMaskConverter,"Applies the mask, then dumps the resulting RecoPulses verbatim.");
    typedef I3MapOMKeyVectorConverter< convert::I3RecoHit > I3RecoHitSeriesMapConverter;
    I3_MAP_CONVERTER_EXPORT_DEFAULT(I3RecoHitSeriesMapConverter,"Dumps all RecoHits verbatim.");
    typedef I3MapOMKeyVectorConverter< convert::I3MCHit > I3MCHitSeriesMapConverter;
    I3_MAP_CONVERTER_EXPORT_DEFAULT(I3MCHitSeriesMapConverter,"Dumps all MCHits verbatim.");
    typedef I3MapOMKeyVectorConverter< convert::pod<double> > I3MapKeyVectorDoubleConverter;
    I3_MAP_CONVERTER_EXPORT_DEFAULT(I3MapKeyVectorDoubleConverter, "Dumps all numbers verbatim");
    typedef I3MapOMKeyVectorConverter< convert::pod<int> > I3MapKeyVectorIntConverter;
    I3_MAP_CONVERTER_EXPORT_DEFAULT(I3MapKeyVectorIntConverter, "Dumps all numbers verbatim");

    I3CONVERTER_EXPORT_DEFAULT(I3MapStringDoubleConverter,"Dumps a std::map<string,double> verbatim");
    I3CONVERTER_EXPORT_DEFAULT(I3MapStringVectorDoubleConverter,"Dumps a std::map<string,vector<double> > verbatim");
    I3CONVERTER_EXPORT_DEFAULT(I3MapStringIntConverter,"Dumps a std::map<string,int> verbatim");
    I3CONVERTER_EXPORT_DEFAULT(I3MapStringBoolConverter,"Dumps a std::map<string,bool> verbatim");

    I3CONVERTER_EXPORT_DEFAULT(I3ParticleConverter,"Dumps an I3Particle verbatim");
    typedef I3TreeConverter<I3ParticleConverter, I3MCTree > I3MCTreeConverter;
    I3CONVERTER_EXPORT_DEFAULT(I3MCTreeConverter,"Dumps the I3MCTree verbatim");

    //--------------------------------------------------------------------------
    // I3PositionConverter
    I3CONVERTER_EXPORT_DEFAULT__WITH_CONVERTER_OBJ(I3PositionConverter,
        "Dumps an I3Position, depending on the, via the option 'BookRefFrame',          \n"
        "specified reference frame.                                                     \n"
        "                                                                               \n"
        "Default Python call signiture:                                                 \n"
        "    icecube.dataclasses.converters.I3PositionConverter(BookRefFrame = icecube.dataclasses.converters.I3PositionConverter.BookRefFrame.Car)\n"
        "                                                                               \n"
        "Options:                                                                       \n"
        "    BookRefFrame:                                                              \n"
        "        Books position data only for the specified reference frame.            \n"
        "                                                                               \n"
        "        type:                                                                  \n"
        "            enum `icecube.dataclasses.converters.I3PositionConverter.BookRefFrame`\n"
        "                                                                               \n"
        "        enum values:                                                           \n"
        "            Car: Cartesian reference frame                                     \n"
        "                 (x, y, z)                                                     \n"
        "            Cyl: Cylindrical reference frame                                   \n"
        "                 (rho, z, phi)                                                 \n"
        "            Sph: Spherical reference frame                                     \n"
        "                 (r, phi, theta)                                               \n"
        "            All: Books position data for all reference frames                  \n"
        "                 (x, y, z, r, rho, phi, theta)                                 \n"
        "                                                                               \n"
        "        default value:                                                         \n"
        "            Car                                                                \n"
        "                                                                               \n"
        "    TableColumnNamePrefix:                                                     \n"
        "        The prefix string that should be prefixed to all table column names.   \n"
        "        This is useful when the converter is used internally by an other       \n"
        "        converter to book an I3Position inside its own table.                  \n"
    );
    {
        I3CONVERTER_CONVERTER_NAMESPACE(I3PositionConverter);
        bp::enum_<I3PositionConverter::BookRefFrame>("BookRefFrame")
          .value("Car", I3PositionConverter::car)
          .value("Cyl", I3PositionConverter::cyl)
          .value("Sph", I3PositionConverter::sph)
          .value("All", I3PositionConverter::all)
          .export_values()
          ;
    }
    I3CONVERTER_CONVERTER_OBJ(I3PositionConverter)
        .def(bp::init<I3PositionConverter::BookRefFrame, std::string>(
            (bp::arg("BookRefFrame")          = I3PositionConverter::car,
             bp::arg("TableColumnNamePrefix") = ""
            )
        ))
        // NOTE: We need to expose the CreateDescription and the FillRows
        //       methods, so they can be also used by other converters written
        //       in Python.
        .def("CreateDescription", &I3PositionConverter::CreateDescription,
            (bp::arg("i3position"))
        )
        .def("FillRows", &I3PositionConverter::FillRows,
            (bp::arg("i3position"), bp::arg("i3tablerow"))
        )
        ;

    //--------------------------------------------------------------------------
    // I3DirectionConverter
    I3CONVERTER_EXPORT_DEFAULT__WITH_CONVERTER_OBJ(I3DirectionConverter,
        "Dumps an I3Direction, depending on the, via the option 'BookRefFrame',         \n"
        "specified reference frame.                                                     \n"
        "                                                                               \n"
        "Default Python call signiture:                                                 \n"
        "    icecube.dataclasses.converters.I3DirectionConverter(BookRefFrame = icecube.dataclasses.converters.I3DirectionConverter.BookRefFrame.Car)\n"
        "                                                                               \n"
        "Options:                                                                       \n"
        "    BookRefFrame:                                                              \n"
        "        Books direction data only for the specified reference frame.           \n"
        "                                                                               \n"
        "        type:                                                                  \n"
        "            enum `icecube.dataclasses.converters.I3DirectionConverter.BookRefFrame`\n"
        "                                                                               \n"
        "        enum values:                                                           \n"
        "            Car: Cartesian reference frame                                     \n"
        "                 (x, y, z)                                                     \n"
        "            Sph: Spherical reference frame                                     \n"
        "                 (azimuth, zenith)                                             \n"
        "            All: Book direction data for all reference frames,                 \n"
        "                 (x, y, z, azimuth, zenith)                                    \n"
        "                                                                               \n"
        "        default value:                                                         \n"
        "            Car                                                                \n"
        "                                                                               \n"
        "    TableColumnNamePrefix:                                                     \n"
        "        The prefix string that should be prefixed to all table column names.   \n"
        "        This is useful when the converter is used internally by an other       \n"
        "        converter to book an I3Position inside its own table.                  \n"
    );
    {
        I3CONVERTER_CONVERTER_NAMESPACE(I3DirectionConverter);
        bp::enum_<I3DirectionConverter::BookRefFrame>("BookRefFrame")
          .value("Car", I3DirectionConverter::car)
          .value("Sph", I3DirectionConverter::sph)
          .value("All", I3DirectionConverter::all)
          .export_values()
          ;
    }
    I3CONVERTER_CONVERTER_OBJ(I3DirectionConverter)
        .def(bp::init<I3DirectionConverter::BookRefFrame, std::string>(
            (bp::arg("BookRefFrame")          = I3DirectionConverter::car,
             bp::arg("TableColumnNamePrefix") = ""
            )
        ))
        // NOTE: We need to expose the CreateDescription and the FillRows
        //       methods, so they can be also used by other converters written
        //       in Python.
        .def("CreateDescription", &I3DirectionConverter::CreateDescription,
            (bp::arg("i3direction"))
        )
        .def("FillRows", &I3DirectionConverter::FillRows,
            (bp::arg("i3direction"), bp::arg("i3tablerow"))
        )
        ;

    typedef I3TreeConverter<convert::I3Trigger> I3TriggerHierarchyConverter;
    I3CONVERTER_EXPORT_DEFAULT(I3TriggerHierarchyConverter,"Dumps all triggers in the I3TriggerHierarchy tree");

    I3CONVERTER_EXPORT_DEFAULT(I3DoubleConverter,"Dumps I3Double objects");
    I3CONVERTER_EXPORT_DEFAULT(I3IntConverter,"Dumps I3Int objects");
    I3CONVERTER_EXPORT_DEFAULT(I3BoolConverter,"Dumps I3Bool objects");

    typedef I3VectorConverter< convert::pod<bool> > I3VectorBoolConverter;
    I3CONVERTER_EXPORT_DEFAULT(I3VectorBoolConverter, "Dumps an I3Vector of bools");

    typedef I3VectorConverter< convert::pod<int16_t> > I3VectorShortConverter;
    I3CONVERTER_EXPORT_DEFAULT(I3VectorShortConverter, "Dumps an I3Vector of shorts");
    typedef I3VectorConverter< convert::pod<uint16_t> > I3VectorUShortConverter;
    I3CONVERTER_EXPORT_DEFAULT(I3VectorUShortConverter, "Dumps an I3Vector of unsigned shorts");

    typedef I3VectorConverter< convert::pod<int32_t> > I3VectorIntConverter;
    I3CONVERTER_EXPORT_DEFAULT(I3VectorIntConverter, "Dumps an I3Vector of ints");
    typedef I3VectorConverter< convert::pod<uint32_t> > I3VectorUIntConverter;
    I3CONVERTER_EXPORT_DEFAULT(I3VectorUIntConverter, "Dumps an I3Vector of unsigned ints");

    typedef I3VectorConverter< convert::pod<I3VectorInt64::value_type> > I3VectorInt64Converter;
    I3CONVERTER_EXPORT_DEFAULT(I3VectorInt64Converter, "Dumps an I3Vector of 64bit ints");
    typedef I3VectorConverter< convert::pod<I3VectorUInt64::value_type> > I3VectorUInt64Converter;
    I3CONVERTER_EXPORT_DEFAULT(I3VectorUInt64Converter, "Dumps an I3Vector of unsigned 64bit ints");

    typedef I3VectorConverter< convert::pod<float> > I3VectorFloatConverter;
    I3CONVERTER_EXPORT_DEFAULT(I3VectorFloatConverter, "Dumps an I3Vector of floats");
    typedef I3VectorConverter< convert::pod<double> > I3VectorDoubleConverter;
    I3CONVERTER_EXPORT_DEFAULT(I3VectorDoubleConverter, "Dumps an I3Vector of doubles");

    typedef I3VectorConverter< convert::OMKey > I3VectorOMKeyConverter;
    I3CONVERTER_EXPORT_DEFAULT(I3VectorOMKeyConverter, "Dumps an I3Vector of OMKeys");

    typedef I3VectorConverter< convert::TankKey > I3VectorTankKeyConverter;
    I3CONVERTER_EXPORT_DEFAULT(I3VectorTankKeyConverter, "Dumps an I3Vector of TankKeys");

    typedef I3VectorConverter< convert::double_pair > I3VectorDoubleDoubleConverter;
    I3CONVERTER_EXPORT_DEFAULT(I3VectorDoubleDoubleConverter, "Dumps an I3Vector of double-double pairs");

    typedef I3VectorConverter< convert::I3FlasherInfo > I3FlasherInfoVectConverter;
    I3CONVERTER_EXPORT_DEFAULT(I3FlasherInfoVectConverter, "Dumps an I3FlasherInfoVect");

    // waveform converter has a non-default constructor
    // don't register this converter, since registered converters need default constructors
    bp::class_<I3WaveformConverter,
    boost::shared_ptr<I3WaveformConverter>,
    bp::bases<I3Converter>,
    boost::noncopyable >("I3WaveformConverter", 
                         "Dumps ATWD and FADC waveforms together", 
                         bp::init<std::string, std::string, bool>())
    ;

    register_converter<I3WaveformSeriesMapConverter>(registry,
						     bp::class_<I3WaveformSeriesMapConverter,
								boost::shared_ptr<I3WaveformSeriesMapConverter>,
								bp::bases<I3Converter>,
								boost::noncopyable >("I3WaveformSeriesMapConverter",
										     "Dumps a single I3WaveformSeriesMap (good for IceTop people not interested in FADC)",
										     bp::init< bp::optional<bool, bool> >(bp::args("calibrate", "bookGeometry"))),
						     true)
      .def("__init__", bp::make_constructor(&::makeWaveformSeriesMapConverter, boost::python::default_call_policies(), (bp::arg("bookGeometry"))));

/*
    // A compilation test: I3MapConverter and I3VectorConverter should be able
    // to accept any I3Converter as a template parameter. Remove comments to
    // check.
    typedef I3Map<OMKey, std::vector<I3Particle> > I3MapKeyVectorI3Particle;
    typedef I3MapOMKeyVectorConverter< I3ParticleConverter > I3MapKeyVectorI3ParticleConverter;
    I3_MAP_CONVERTER_EXPORT_DEFAULT(I3MapKeyVectorI3ParticleConverter, "Dumps all particles ... just a compilation test");
*/
    typedef I3VectorConverter< I3ParticleConverter > I3VectorI3ParticleConverter;
    I3CONVERTER_EXPORT_DEFAULT(I3VectorI3ParticleConverter, "Dumps an I3Vector<I3Particle>");

    I3CONVERTER_EXPORT_DEFAULT(I3FilterResultMapConverter,
		       "Dumps the results of each Pole filter to a table column.");


}
