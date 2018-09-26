#include <string>

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <QColor>
#include <QFont>

#include "shovelart/Artist.h"
#include "shovelart.h"
#include "shovelart/ColorVariant.h"
#include "shovelart/SceneVariant.h"
#include "shovelart/graphicsmath.h"
#include "shovelart/RangeSetting.h"
#include "shovelart/ChoiceSetting.h"
#include "shovelart/KeySetting.h"
#include "shovelart/FileSetting.h"
#include "shovelart/OMKeySet.h"

#include "scripting/vec3d_converter.h"
#include "scripting/qstring_converter.h"
#include "scripting/qstringlist_converter.h"
#include "scripting/pytypename.h"

#include "dataclasses/I3Direction.h"

// has to be last, because it overwrites a template specialization
#include "scripting/pycompat.h"

namespace scripting{ namespace shovelart{

namespace bp = boost::python;

boost::shared_ptr<RangeSetting> RangeSetting_init( float vmin, float vmax, int steps, bp::object obj ){
    bp::extract<float> get_float(obj);
    if( get_float.check() ){
        return boost::shared_ptr<RangeSetting>(
            new RangeSetting( vmin, vmax, steps, get_float() )
        );
    } else {
        // this constructor is deprecated and should be phased out by the end of 2015
        return boost::shared_ptr<RangeSetting>(
            new RangeSetting( vmin, vmax, steps, bp::extract<int>(obj) )
        );
    }
}

boost::shared_ptr<ChoiceSetting> ChoiceSetting_init( const bp::object& obj, int val ){
    boost::shared_ptr<ChoiceSetting> result;
    bp::extract<QString> get_str(obj);
    if( get_str.check() ){
        // obj is a string, call string-based constructor
        result.reset( new ChoiceSetting(get_str(), val) );
    }else{
        QStringList opts;
        // assume that obj is a sequence
        for (int i = 0; i < bp::len(obj); ++i)
            opts.push_back( bp::extract<QString>(obj[i]) );
        result.reset( new ChoiceSetting(opts, val) );
    }
    return result;
}

boost::shared_ptr<KeySetting> KeySetting_init( const bp::object& obj, const std::string& key ){
    const QString val = QString::fromStdString( key );

    // check whether obj is a sequence
    if ( PySequence_Check(obj.ptr()) ){
        std::vector<std::string> type_names;
        for (int i = 0; i < bp::len(obj); ++i){
            const bp::object& o = obj[i];
            bp::extract<std::string> get_str( o );
            std::string tname;
            if ( get_str.check() ){
                // is a string, assume it is a type name
                tname = get_str();
            } else {
                // assume it is a I3FrameObject
                tname = type_name( o );
            }
            type_names.push_back(tname);
        }
        return boost::shared_ptr<KeySetting>( new KeySetting(type_names, val) );
    }

    // obj is not iterable --> either str or I3FrameObject
    bp::extract<std::string> get_str( obj );
    std::string tname;
    if( get_str.check() ){
        tname = get_str();
    } else {
        tname = type_name( obj );
    }

    return boost::shared_ptr<KeySetting>( new KeySetting(tname, val) );
}

bp::tuple ChoiceSetting_options(const ChoiceSetting& s){
    QStringList lst = s.options();
    bp::list pylst;
    for( int i = 0; i < lst.size(); ++i )
        pylst.append( lst[i] );
    return bp::tuple(pylst);
}

std::string vec3d_repr( const vec3d& v ){
    std::stringstream buf;
    buf << "vec3d(" << v.x() << ", " << v.y() << ", " << v.z() << ")";
    return buf.str();
}

bool vec3d_less( const vec3d& a, const vec3d& b ){
    if( a.x() < b.x() )
        return true;
    if( a.x() > b.x() )
        return false;
    // equal x
    if( a.y() < b.y() )
        return true;
    if( a.y() > b.y() )
        return false;
    // equal y
    if( a.z() < b.z() )
        return true;
    return false;
}

qreal vec3d_getitem( const vec3d& v, unsigned idx ){
    switch( idx ){
        case 0: return v.x();
        case 1: return v.y();
        case 2: return v.z();
        default: throw std::out_of_range("idx must be < 3");
    }
    return 0;
}

std::string PyQColor_repr( const QColor& qc ){
    std::stringstream buf;
    buf << "PyQColor(" << qc.red() << "," << qc.green() << "," << qc.blue() << "," << qc.alpha() << ")";
    return buf.str();
}

bp::tuple PyQColor_rgb( const QColor& qc ){
    return bp::make_tuple( qc.red(), qc.green(), qc.blue() );
}

bp::tuple PyQColor_rgbF( const QColor& qc ){
    return bp::make_tuple( qc.red() / 255. , qc.green() / 255. , qc.blue() / 255. );
}

std::string PyQFont_repr( const QFont& font ){
    std::stringstream buf;
    buf << "PyQFont.fromString('" << font.toString().toStdString() << "')";
    return buf.str();
}

QFont PyQFont_fromStr( const QString& str ){
    QFont ret;
    ret.fromString(str);
    return ret;
}

std::ostream& operator<<( std::ostream& out, const SceneOverlay::SizeHint& hint ){
    out << "OverlaySizeHint(" << hint.x << "," << hint.y << "," << hint.width << "," << hint.height << ")";
    return out;
}

// In this context only, output strings with quote marks. 
// This is to get the repr wrappers right.
std::ostream& operator<<( std::ostream& os, const std::string& s ){
    os << "'" << s.c_str() << "'" ;
    return os;
}

std::ostream& operator<<( std::ostream& os, const QString& x ){
    os << x.toStdString();
    return os;
}

template<typename T>
std::string ComplexSetting_repr( const T& s ){
    // we cheat, since this is not actually a string representation of the type
    std::ostringstream os;
    os << static_cast<typename T::Value>(s);
    return os.str();
}

template<typename T>
typename T::Value ComplexSetting_value( const T& s ){
    return s;
}

/** Python __repr__ for types with bp::vector_indexing_suites */
template < typename T >
std::string list_repr( const T& t ){
    std::stringstream str;
    str << "[";
    for( typename T::const_iterator i = t.begin(); i != t.end(); ++i ){
        str << *i << ", ";
    }
    str << "]";
    return str.str();
}


// Used to create vec3ds from I3Position or I3Direction
template <typename T>
boost::shared_ptr<vec3d>
from_pseudovector(const T &t){
    return boost::shared_ptr<vec3d>(new vec3d(t.GetX(), t.GetY(), t.GetZ()));
}

void export_shovelart_types(){

    // Automatic anything -> vec3d conversion
    AnythingToVec3d();

    // Automatic QString <-> PyString conversion
    QStringConverter();

    // Automatic QStringList <-> PyString list conversion
    QStringListConverter();

    /* vec3d */
    bp::class_< vec3d > ( "vec3d", bp::init<double,double,double>() )
        .def("__init__", bp::make_constructor(&from_pseudovector<I3Position>))
        .def("__init__", bp::make_constructor(&from_pseudovector<I3Direction>))
        .def("__repr__", vec3d_repr )
        .def("__lt__", vec3d_less )
        .def("__getitem__", vec3d_getitem )
        .def_readwrite( "x", &vec3d::x, "x component" )
        .def_readwrite( "y", &vec3d::y, "y component" )
        .def_readwrite( "z", &vec3d::z, "z component" )
        .def("mag", &vec3d::length, "length of the vector" )
        .def( bp::self + bp::self )
        .def( bp::self - bp::self )
        // scalar multiplication with floats
        .def( bp::self * double() )
        .def( double() * bp::self )
        .def( bp::self / double() )
        .def( bp::self *= double() )
        .def( bp::self /= double() )
        // and integers for completeness
        .def( bp::self * int() )
        .def( int() * bp::self )
        .def( bp::self / int() )
        .def( bp::self *= int() )
        .def( bp::self /= int() )
    ;

    bp::class_< std::vector<vec3d> > ( "Vec3dList" )
        .def( bp::vector_indexing_suite<std::vector<vec3d> >() );

    /* Keylists are just a fancy list of strings */
    bp::class_< ArtistKeyList >( "ArtistKeylist" )
        .def( bp::vector_indexing_suite<ArtistKeyList>() )
        .def( "__repr__", list_repr< ArtistKeyList > );

    /* Selection contents */
    bp::class_<OMKeySet>( "OMKeySet",
            "A set of OMKeys, backed by a std::vector.  Used as a PyArtist setting to indicate "
            "that the artist wishes to track the currently selected OMs in the GUI.")
        .def( bp::vector_indexing_suite< OMKeySet >() )
        .def( "__repr__", list_repr<OMKeySet> );

    bp::class_<
        RangeSetting,
        boost::shared_ptr<RangeSetting> 
    >( "RangeSetting",
       "A range setting as defined by a min and max, a number of steps, and initial value",
       bp::no_init )
        .def( "__init__", bp::make_constructor(RangeSetting_init) )
        .def( "__repr__", ComplexSetting_repr<RangeSetting> )
        .add_property( "value", ComplexSetting_value<RangeSetting> )
    ;

    bp::class_<
        ChoiceSetting,
        boost::shared_ptr<ChoiceSetting>
    >( "ChoiceSetting",
       "An integer choice from a collection of options",
       bp::no_init )
        .def( "__init__", bp::make_constructor(ChoiceSetting_init) )
        .def( "__repr__", ComplexSetting_repr<ChoiceSetting> )
        .add_property( "value", ComplexSetting_value<ChoiceSetting> )
        .add_property( "options", ChoiceSetting_options )
    ;

    bp::class_<
        KeySetting,
        boost::shared_ptr<KeySetting>
    >( "KeySetting",
       "A key setting for a given type of I3FrameObjects",
       bp::no_init)
        .def( "__init__", bp::make_constructor(KeySetting_init) )
        .def( "__repr__", ComplexSetting_repr<KeySetting> )
        .add_property( "value", ComplexSetting_value<KeySetting> )
    ;

    bp::class_<
        FileSetting,
        boost::shared_ptr<FileSetting>
    >( "FileSetting",
       "A filename setting",
       bp::init<QString>( bp::arg("filename") ) )
        .def( "__repr__", ComplexSetting_repr<FileSetting> )
        .add_property( "value", ComplexSetting_value<FileSetting> )
    ;

    /* Color maps */
    bp::class_<
        ColorMap,
        ColorMapPtr,
        boost::noncopyable
    >( "ColorMap", bp::no_init )
        .def("value", &ColorMap::value )
        .def("__repr__", &ColorMap::toString )
        .def("getPythonMapByName", &ColorMap::getPythonMapByName )
        .staticmethod( "getPythonMapByName" );
    ;

    bp::class_< I3TimeColorMap, bp::bases<ColorMap> >( "I3TimeColorMap" );

    bp::class_< ConstantColorMap, bp::bases<ColorMap> >( "ConstantColorMap", bp::init<QColor>() );

    /* Colors */
    bp::class_< QColor >( "PyQColor", bp::init<int,int,int,int>( (bp::arg("r"), bp::arg("g"), bp::arg("b"), bp::arg("a")=255) ) )
        .def("fromRgb", (QColor(*)(int,int,int,int))&QColor::fromRgb,
             (bp::arg("r"), bp::arg("g"), bp::arg("b"), bp::arg("a")=255) )
        .staticmethod("fromRgb")
        .def("fromRgbF", (QColor(*)(qreal,qreal,qreal,qreal))&QColor::fromRgbF,
             (bp::arg("r"), bp::arg("g"), bp::arg("b"), bp::arg("a")=1.0) )
        .staticmethod("fromRgbF")
        .def("__repr__", PyQColor_repr )
        .add_property( "red", &QColor::red, &QColor::setRed )
        .add_property( "green", &QColor::green, &QColor::setGreen )
        .add_property( "blue", &QColor::blue, &QColor::setBlue )
        .add_property( "alpha", &QColor::alpha, &QColor::setAlpha )
        .def("rgb", PyQColor_rgb )
        .def("rgbF", PyQColor_rgbF )
    ;

    /* Fonts */
    bp::class_<
        QFont,
        boost::shared_ptr<QFont>
    >( "PyQFont" )
        .add_property( "pointSize", &QFont::pointSize, &QFont::setPointSize )
        .def( "__repr__", &PyQFont_repr )
        .def( "fromString", &PyQFont_fromStr ).staticmethod( "fromString" )
        .def( "toString", &QFont::toString )
    ;

    /* Overlay size info */
    bp::class_<SceneOverlay::SizeHint>( "OverlaySizeHint",
            "A read-only value containing the size in pixels of a screen overlay",
            bp::init<int,int,int,int>( (bp::arg("xpos"), bp::arg("ypos"), bp::arg("width"), bp::arg("height") ) ) )
        .def_readonly( "width", &SceneOverlay::SizeHint::width, "Pixel width of the overlay" )
        .def_readonly( "height", &SceneOverlay::SizeHint::height, "Pixel height of the overlay" )
        .def_readonly( "xpos", &SceneOverlay::SizeHint::x, "x pixel position of overlay, measured from the left" )
        .def_readonly( "ypos", &SceneOverlay::SizeHint::y, "y pixel position of the overlay, measured from the top" )
    ;

    bp::class_<SceneState::OverlayHints>( "OverlaySizeHints",
            "A list of OverlaySizeHints, backed by a std::vector.")
        .def( bp::vector_indexing_suite< SceneState::OverlayHints>() )
        .def( "__repr__", list_repr<SceneState::OverlayHints> )
    ;

    // this arrives magically from cmake/tool-patches, and allows a python list of overlay hints
    // to magically become an OverlayHints object
    from_python_sequence<SceneState::OverlayHints, variable_capacity_policy>();
}

}} // namespace scripting::shovelart
